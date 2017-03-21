#include "AgentProtocol.h"
#include "Gate.h"
#include "IHarbor.h"
#include "IAgent.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "IDB.h"
#include "IIdmgr.h"
#include "DBDef.h"

bool Gate::initialize(sl::api::IKernel * pKernel){
	_self = this;
	return true;
}

bool Gate::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_db, DB);
	FIND_MODULE(_IdMgr, IdMgr);

	RGS_NODE_HANDLER(_harbor, NodeProtocol::SCENEMGR_MSG_DISTRIBUTE_LOGIC_ACK, Gate::onSceneMgrDistributeLogic);

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

void Gate::reset(sl::api::IKernel* pKernel, int64 id, int8 state){
	SLASSERT(state == GATE_STATE_NONE || state == GATE_STATE_ROLELOADED, "wtf");
	Player& player = _players[id];
	int8 oldState = player.state;
	if (oldState > GATE_STATE_ROLELOADED){
		SLASSERT(player.logic > 0 && player.selectActorId != 0, "wtf");

		IArgs<1, 32> args;
		args << player.selectActorId;
		args.fix();
		_harbor->send(NodeType::LOGIC, player.logic, NodeProtocol::GATE_MSG_UNBIND_PLAYER_REQ, args.out());

		_actors.erase(player.selectActorId);
		_logicPlayers[player.logic].erase(id);

		player.selectActorId = 0;
		player.logic = 0;
	}

	if (oldState > GATE_STATE_NONE && state == GATE_STATE_NONE){
		IArgs<1, 32> args;
		args << player.agentId << player.accountId;
		args.fix();
		_harbor->send(NodeType::ACCOUNT, 1, NodeProtocol::GATE_MSG_UNBIND_ACCOUNT_REQ, args.out());

		player.accountId = 0;
		player.roles.clear();
	}

	player.state = state;
}

void Gate::sendToClient(sl::api::IKernel* pKernel, const int64 id, const int32 msgId, const OBStream& buf){
	int32 header[2];
	header[0] = msgId;
	header[1] = buf.getSize() + sizeof(int32)* 2;

	_agent->send(id, header, sizeof(header));
	_agent->send(id, buf.getContext(), buf.getSize());
}

void Gate::onSceneMgrDistributeLogic(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int64 agentId = args.getInt64(0);
	int64 actorId = args.getInt64(1);
	int32 logic = args.getInt32(2);

	if (_players.find(agentId) != _players.end()){
		if (logic > 0){
			Player& player = _players[agentId];
			SLASSERT(player.state == GATE_STATE_DISTRIBUTE, "wtf");

			player.selectActorId = actorId;
			player.logic = logic;
			player.state = GATE_STATE_BINDING;

			_actors[actorId] = agentId;
			_logicPlayers[logic].insert(agentId);

			IArgs<2, 32> args;
			args << actorId << player.accountId;
			args.fix();
			_harbor->send(NodeType::LOGIC, logic, NodeProtocol::GATE_MSG_BIND_PLAYER_REQ, args.out());
		}
		else{
			reset(pKernel, agentId, GATE_STATE_ROLELOADED);

			IBStream<128> buf;
			buf << (int32)ProtocolError::ERROR_DISTRIBUTE_LOGIC_FAILED;
			sendToClient(pKernel, agentId, AgentProtocol::SERVER_MSG_SELECT_ROLE_RSP, buf.out());
		}
	}
}

void Gate::onClientLoginReq(sl::api::IKernel* pKernel, const int64 id, const OBStream& args){
	const char* accountName = nullptr;
	if (!args.readString(accountName))
		return;

	SLASSERT(_players.find(id) != _players.end(), "not find agent!");
	Player& player = _players[id];

	if (player.state != GATE_STATE_NONE)
		return;

	AccountInfo info{ accountName };
	auto callor = CREATE_DB_CALL_CONTEXT(_db, 0, id, &info, sizeof(info));
	callor->query("account", [&](sl::api::IKernel* pKernel, IDBQueryParamAdder* adder, IDBCallCondition* condition){
		adder->AddColumn("id");
		condition->AddCondition("name", IDBCallCondition::DBConditionOpType::DBOP_EQ, accountName);
	}, CALLOR_CB(Gate::onQueryAccountCB));
}

void Gate::onQueryAccountCB(sl::api::IKernel* pKernel, const int64 id, const bool success, const int32 affectedRow, const IDBCallSource* source, const IDBResult* result){
	if (!success){
		SLASSERT(false, "wtf");
		return;
	}

	int64 accountId = 0;
	if (result->rowCount() > 0){
		accountId = result->getDataInt64(0, "id");
	}else{
		AccountInfo* info = (AccountInfo*)source->getContext(sizeof(AccountInfo));
		accountId = (int64)_IdMgr->allocID();
		
		auto callor = CREATE_DB_CALL(_db, 0, 0);
		callor->insert("account", [&](sl::api::IKernel* pKernel, IDBInsertParamAdder* adder){
			adder->AddColumn("id", accountId);
			adder->AddColumn("name", info->name.c_str());
		}, nullptr);
	}

	Player& player = _players[id];
	player.accountId = accountId;
	player.state = GATE_STATE_AUTHENING;

	IArgs<3, 128> args;
	args << player.agentId << player.accountId << 0;
	args.fix();

	_harbor->send(NodeType::ACCOUNT, 1, NodeProtocol::GATE_MSG_BIND_ACCOUNT_REQ, args.out());
}

void Gate::onClientSelectRoleReq(sl::api::IKernel* pKernel, const int64 id, const OBStream& args){
	int64 actorId = 0;
	if (!args.read(actorId))
		return;

	SLASSERT(_players.find(id) != _players.end(), "not find agent");
	Player& player = _players[id];

	if (player.state == GATE_STATE_ROLELOADED){
		if (std::find_if(player.roles.begin(), player.roles.end(), [actorId](const Role& role){
			return role.actorId == actorId;
		}) == player.roles.end()){
			return;
		}

		player.state = GATE_STATE_DISTRIBUTE;

		IArgs<2, 32> args;
		args << id << actorId;
		args.fix();
		_harbor->send(NodeType::SCENEMGR, 1, NodeProtocol::GATE_MSG_DISTRIBUTE_LOGIC_REQ, args.out());
	}
}

void Gate::test(){
	auto dbCall = CREATE_DB_CALL(_db, 0, 0);
	dbCall->insert("user", [&](sl::api::IKernel* pKernel, IDBInsertParamAdder* adder){
		adder->AddColumn("id", 877777888888);
		adder->AddColumn("name", "ddc");
	}, nullptr);
}