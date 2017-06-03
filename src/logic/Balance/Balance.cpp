#include "Balance.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "IAgent.h"
#include "slbinary_stream.h"
#include "ProtocolID.pb.h"
#include "slxml_reader.h"

#define LOGIN_ALLOW_INTERVAL 30000 
bool Balance::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	_self = this;

	_maxTicketSize = 0;
	_maxClientSize = 0;
	_nowClients = 0;
	_overClients = 0;
	_currentTicket = 0;
	_passTicket = 0;

	sl::XmlReader svrConf;
	if (!svrConf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "cant load core file");
		return false;
	}
	_maxTicketSize = svrConf.root()["balance"][0].getAttributeInt32("max_tickets");
	_maxClientSize = svrConf.root()["balance"][0].getAttributeInt32("max_clients");

	return true;
}

bool Balance::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_agent, Agent);

	_harbor->addNodeListener(this);
	_agent->setListener(this);

	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::GATE_MSG_GATE_REGISTER, Balance::onGateRegister);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::GATE_MSG_LOAD_REPORT, Balance::onGateLoadReport);

	START_TIMER(_self, 0, TIMER_BEAT_FOREVER, LOGIN_ALLOW_INTERVAL);

	return true;
}

bool Balance::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Balance::onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId){
	if (nodeType == NodeType::GATE){
		_gates.erase(nodeId);
	}
}

void Balance::onAgentOpen(sl::api::IKernel* pKernel, const int64 id){
	if (_maxTicketSize > 0){
		if (_tickets.size() >= _maxTicketSize){
			_agent->kick(id);
			return;
		}
		
		int32 ticket = getEntryTicket();
		if (ticket > 0){
			_tickets[ticket] = id;
			_agentTickets[id] = ticket;
			sendTicket(pKernel, id, ticket);
			return;
		}
	}

	sendGate(pKernel, id);
}

void Balance::onAgentClose(sl::api::IKernel* pKernel, const int64 id){
	if (_maxTicketSize > 0){
		auto itor = _agentTickets.find(id);
		if (itor != _agentTickets.end()){
			_tickets.erase(itor->second);
			_agentTickets.erase(itor);
		}
	}
}

void Balance::onGateRegister(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	SLASSERT(_gates.find(nodeId) == _gates.end(), "wtf");
	const char* ip = args.getString(0);
	const int32 port = args.getInt32(1);
	ECHO_TRACE("new gate node[%d] register", nodeId);

	GateInfo& gate = _gates[nodeId];
	gate.nodeId = nodeId;
	gate.ip = ip;
	gate.port = port;
	gate.load = 0;
}

void Balance::onGateLoadReport(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	auto itor = _gates.find(nodeId);
	if (itor == _gates.end()){
		SLASSERT(false, "wtf");
		return;
	}

	const int32 load = args.getInt32(0);
	GateInfo& info = _gates[nodeId];
	info.load = load;
}

void Balance::onTime(sl::api::IKernel* pKernel, int64 timetick){
	_overClients = 0;
	_nowClients = 0;
	for (auto itor = _gates.begin(); itor != _gates.end(); ++itor){
		_nowClients += itor->second.load;
	}

	if (_currentTicket > _passTicket){
		if (_maxClientSize > _nowClients){
			int32 passTicket = _currentTicket - _passTicket;
			int32 passClient = _maxClientSize - _nowClients;
			if (passClient > passTicket)
				_passTicket = _currentTicket;
			else{
				_passTicket += passClient;
			}

			for (auto itr = _tickets.begin(); itr != _tickets.end(); ++itr){
				if (itr->first <= _passTicket){
					if (_agentTickets.find(itr->second) != _agentTickets.end()){
						sendGate(pKernel, itr->second);
						++_overClients;
					}
				}
				else
					sendTicket(pKernel, itr->second, itr->first);
			}
		}
	}
}


void Balance::sendGate(sl::api::IKernel* pKernel, int64 agentId){
	SLASSERT(!_gates.empty(), "wtf");
	GateInfo* gate = nullptr;
	for (auto itor = _gates.begin(); itor != _gates.end(); ++itor){
		if (gate == nullptr || gate->load > itor->second.load)
			gate = &(itor->second);
	}

	if (gate == nullptr){
		_agent->kick(agentId);
		return;
	}

	IArgs<1, 64> args;
	args << agentId;
	args.fix();
	_harbor->send(NodeType::GATE, gate->nodeId, NodeProtocol::BALANCE_MSG_SYNC_LOGIN_TICKET, args.out());

	sl::IBStream<128> ack;
	ack << gate->ip.c_str() << gate->port << agentId;
	sl::OBStream out = ack.out();

	client::Header header;
	header.messageId = ServerMsgID::SERVER_MSG_GIVE_GATE_ADDRESS_RSP;
	header.size = sizeof(client::Header) + out.getSize();
	_agent->send(agentId, &header, sizeof(header));
	_agent->send(agentId, out.getContext(), out.getSize());
}

void Balance::sendTicket(sl::api::IKernel* pKernel, int64 agentId, int32 ticket){
	sl::IBStream<256> ack;
	ack << (ticket - _passTicket);
	sl::OBStream out = ack.out();

	client::Header header;
	header.messageId = ServerMsgID::SERVER_MSG_NOTIFY_TICKET_INFO_RSP;
	header.size = sizeof(client::Header) + out.getSize();
	_agent->send(agentId, &header, sizeof(header));
	_agent->send(agentId, out.getContext(), out.getSize());
}

int32 Balance::getEntryTicket(){
	if (_currentTicket == _passTicket){
		if ((_nowClients + _overClients) < _maxClientSize){
			++_overClients;
			return 0;
		}
		return ++_currentTicket;
	}
	else
		return ++_currentTicket;
}





