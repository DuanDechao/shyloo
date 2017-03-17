#include "AgentProtocol.h"
#include "Gate.h"
#include "IHarbor.h"
#include "IAgent.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "IDB.h"

bool Gate::initialize(sl::api::IKernel * pKernel){
	_self = this;
	return true;
}

bool Gate::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_db, DB);
	
	_self->rgsAgentMessageHandler(AgentProtocol::CLIENT_MSG_LOGIN_REQ, &Gate::onClientLoginReq);
	
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
	SLASSERT(_players.find(id) == _players.end(), "duplicate agent id£º%d", id);
	_players[id] = { id, 0, 0, 0, GATE_STATE_NONE };
}

void Gate::onAgentClose(sl::api::IKernel* pKernel, const int64 id){
	SLASSERT(_players.find(id) != _players.end(), "where is agent %d", id);

	if (_players[id].state != GATE_STATE_NONE){

	}

	_players.erase(id);
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
	if (_gateProtos.find(msgId) != _gateProtos.end()){
		OBStream buf((const char*)context + sizeof(int32)* 2, len);
		(this->*_gateProtos[msgId])(pKernel, id, buf);
	}
	else{
		if (_players[id].state == GATE_STATE_ONLINE){
			transMsgToLogic(pKernel, id, context, len);
		}
	}
	return len;
}

void Gate::rgsAgentMessageHandler(int32 messageId, agent_args_cb handler){
	SLASSERT(_gateProtos.find(messageId) == _gateProtos.end(), "duplicate agent msg %d", messageId);
	
	_gateProtos[messageId] = handler;
}

void Gate::transMsgToLogic(sl::api::IKernel* pKernel, const int64 id, const void* pContext, const int32 size){
	Player& player = _players[id];
	_harbor->prepareSend(NodeType::LOGIC, player.logic, NodeProtocol::GATE_MSG_TRANSMIT_MSG_TO_LOGIC, size + sizeof(int64));
	_harbor->send(NodeType::LOGIC, player.logic, &id, sizeof(id));
	_harbor->send(NodeType::LOGIC, player.logic, pContext, size);
}

void Gate::onClientLoginReq(sl::api::IKernel* pKernel, const int64 id, const OBStream& args){

}

void Gate::test(){
	auto dbCall = CREATE_DB_CALL(_db, 0, 0);
	dbCall->insert("user", [&](sl::api::IKernel* pKernel, IDBInsertParamAdder* adder){
		adder->AddColumn("id", 877777888888);
		adder->AddColumn("name", "ddc");
	}, nullptr);
}