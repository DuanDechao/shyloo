#include "AgentProtocol.h"
#include "Gate.h"
#include "IHarbor.h"
#include "IAgent.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
Gate* Gate::s_gate = nullptr;
IHarbor* Gate::s_harbor = nullptr;
sl::api::IKernel* Gate::s_kernel = nullptr;
IAgent*	Gate::s_agent = nullptr;
IDB* Gate::s_db = nullptr;
std::unordered_map<int64, Gate::Player> Gate::s_players;
std::unordered_map<int32, std::unordered_set<int64>> Gate::s_logicPlayers;
std::unordered_map<int32, Gate::agent_args_cb> Gate::s_gateProtos;

bool Gate::initialize(sl::api::IKernel * pKernel){
	s_gate = this;
	return true;
}

bool Gate::launched(sl::api::IKernel * pKernel){
	s_harbor = (IHarbor*)pKernel->findModule("Harbor");
	SLASSERT(s_harbor, "not find module harbor");
	s_db = (IDB*)pKernel->findModule("DB");
	SLASSERT(s_db, "not find module s_db");

	//s_db->rgsDBTaskCallBack(32, Gate::queryCB);

	s_gate->rgsAgentMessageHandler(AgentProtocol::CLIENT_MSG_LOGIN_REQ, &Gate::onClientLoginReq);

	test();

	return true;
}
bool Gate::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Gate::onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){

}
void Gate::onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId){

}

void Gate::onAgentOpen(sl::api::IKernel* pKernel, const int64 id){
	SLASSERT(s_players.find(id) == s_players.end(), "duplicate agent id£º%d", id);
	s_players[id] = { id, 0, 0, 0, GATE_STATE_NONE };
}

void Gate::onAgentClose(sl::api::IKernel* pKernel, const int64 id){
	SLASSERT(s_players.find(id) != s_players.end(), "where is agent %d", id);

	if (s_players[id].state != GATE_STATE_NONE){

	}

	s_players.erase(id);
}

int32 Gate::onAgentRecv(sl::api::IKernel* pKernel, const int64 id, const void* context, const int32 size){
	if (size < sizeof(int32)* 2){
		return 0;
	}

	int32 len = *((int32*)((const char*)context + sizeof(int32)));
	if (size < len){
		return 0;
	}

	int32 msgId = *((int32*)context);
	if (s_gateProtos.find(msgId) != s_gateProtos.end()){
		OBStream buf((const char*)context + sizeof(int32)* 2, len);
		(this->*s_gateProtos[msgId])(pKernel, id, buf);
	}
	else{
		if (s_players[id].state == GATE_STATE_ONLINE){
			transMsgToLogic(pKernel, id, context, len);
		}
	}
	return len;
}

void Gate::rgsAgentMessageHandler(int32 messageId, agent_args_cb handler){
	SLASSERT(s_gateProtos.find(messageId) == s_gateProtos.end(), "duplicate agent msg %d", messageId);
	
	s_gateProtos[messageId] = handler;
}

void Gate::transMsgToLogic(sl::api::IKernel* pKernel, const int64 id, const void* pContext, const int32 size){
	Player& player = s_players[id];
	s_harbor->prepareSend(NodeType::LOGIC, player.logic, NodeProtocol::GATE_MSG_TRANSMIT_MSG_TO_LOGIC, size + sizeof(int64));
	s_harbor->send(NodeType::LOGIC, player.logic, &id, sizeof(id));
	s_harbor->send(NodeType::LOGIC, player.logic, pContext, size);
}

void Gate::onClientLoginReq(sl::api::IKernel* pKernel, const int64 id, const OBStream& args){

}


void Gate::test(){
	static int64 uid = 3333253457678888;
	IArgs<2, 128> args;
	args << uid;
	args << "fsdfgsdg";
	args.fix();
	CALL_DB(s_db, NEW testDBTask(), args.out());
}

void Gate::queryCB(sl::api::IKernel* pKernel, const sl::api::ICacheDataResult& result){
	ECHO_ERROR("call back sucess!");
}