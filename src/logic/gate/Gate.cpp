#include "Gate.h"
#include "IAgent.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "ICacheDB.h"
#include "IIdmgr.h"
#include "DBDef.h"
#include "IRoleMgr.h"
#include "ProtocolID.pb.h"
#include "Protocol.pb.h"

bool Gate::initialize(sl::api::IKernel * pKernel){
	_self = this;
	return true;
}

bool Gate::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_cacheDB, CacheDB);
	FIND_MODULE(_IdMgr, IdMgr);
	FIND_MODULE(_roleMgr, RoleMgr);
	FIND_MODULE(_agent, Agent);

	_agent->setListener(this);

	_maxRoleNum = 4;

	RGS_NODE_HANDLER(_harbor, NodeProtocol::SCENEMGR_MSG_DISTRIBUTE_LOGIC_ACK, Gate::onSceneMgrDistributeLogic);
	RGS_NODE_HANDLER(_harbor, NodeProtocol::ACCOUNT_MSG_BIND_ACCOUNT_ACK, Gate::onAccountBindAccountAck);
	RGS_NODE_HANDLER(_harbor, NodeProtocol::ACCOUNT_MSG_KICK_FROM_ACCOUNT, Gate::onAccountKickFromAccount);
	RGS_NODE_HANDLER(_harbor, NodeProtocol::LOGIC_MSG_BIND_PLAYER_ACK, Gate::onLogicBindPlayerAck);

	_self->rgsAgentMessageHandler(ClientMsgID::CLIENT_MSG_LOGIN_REQ, &Gate::onClientLoginReq);
	_self->rgsAgentMessageHandler(ClientMsgID::CLIENT_MSG_SELECT_ROLE_REQ, &Gate::onClientSelectRoleReq);
	_self->rgsAgentMessageHandler(ClientMsgID::CLIENT_MSG_CREATE_ROLE_REQ, &Gate::onClientCreateRoleReq);
	
	test();
	return true;
}
bool Gate::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Gate::onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId){
	if (nodeType == NodeType::ACCOUNT){
		std::unordered_map<int64, Player> temp(_players);
		for (auto& itor = temp.begin(); itor != temp.end(); ++itor){
			_agent->kick(itor->first);
		}
	}
	else if (nodeType == NodeType::LOGIC){
		std::unordered_set<int64> temp(_logicPlayers[nodeId]);
		for (auto agentId : temp){
			_agent->kick(agentId);
		}
	}
}

void Gate::onAgentOpen(sl::api::IKernel* pKernel, const int64 id){
	SLASSERT(_players.find(id) == _players.end(), "duplicate agent id£º%d", id);
	_players[id] = { id, 0, 0, 0, GATE_STATE_NONE };
}

void Gate::onAgentClose(sl::api::IKernel* pKernel, const int64 id){
	SLASSERT(_players.find(id) != _players.end(), "where is agent %d", id);

	if (_players[id].state != GATE_STATE_NONE){
		reset(pKernel, id, GATE_STATE_NONE);
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
			buf << (int32)ErrorCode::ERROR_DISTRIBUTE_LOGIC_FAILED;
			sendToClient(pKernel, agentId, ServerMsgID::SERVER_MSG_SELECT_ROLE_RSP, buf.out());
		}
	}
}

void Gate::onAccountBindAccountAck(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int64 agentId = args.getInt64(0);
	int64 accountId = args.getInt64(1);
	int32 errorCode = args.getInt32(2);

	if (_players.find(agentId) != _players.end()){
		Player& player = _players[agentId];
		SLASSERT(player.state == GATE_STATE_AUTHENING && player.accountId == accountId, "wtf");
		if (player.accountId != accountId)
			return;

		if (errorCode == ErrorCode::ERROR_NO_ERROR){
			bool ret = _roleMgr->getRoleList(accountId, [&player](sl::api::IKernel* pKernel, const int64 actorId, IRole* role){
				player.roles.push_back({ actorId, role });
			});

			if (ret){
				IBStream<4096> buf;
				buf << (int32)ErrorCode::ERROR_NO_ERROR << (int32)player.roles.size();
				for (const auto& role : player.roles){
					buf << role.actorId;
				}
				sendToClient(pKernel, player.agentId, ServerMsgID::SERVER_MSG_LOGIN_RSP, buf.out());

				player.state = GATE_STATE_ROLELOADED;
			}
			else{
				reset(pKernel, agentId, GATE_STATE_NONE);
				
				IBStream<128> buf;
				buf << (int32)ErrorCode::ERROR_GET_ROLE_LIST_FAILED;
				sendToClient(pKernel, player.agentId, ServerMsgID::SERVER_MSG_LOGIN_RSP, buf.out());
			}
		}
		else{
			IBStream<128> buf;
			buf << errorCode;
			sendToClient(pKernel, player.agentId, ServerMsgID::SERVER_MSG_LOGIN_RSP, buf.out());
		}
	}
}

void Gate::onAccountKickFromAccount(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int64 agentId = args.getInt64(0);
	if (_players.find(agentId) != _players.end()){
		Player& player = _players[agentId];
		SLASSERT(player.state >= GATE_STATE_ROLELOADED, "wtf");

		reset(pKernel, agentId, GATE_STATE_NONE);
	}
}

void Gate::onLogicBindPlayerAck(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int32 errCode = args.getInt32(0);
	int64 actorId = args.getInt64(1);
	int64 accountId = args.getInt64(2);
	
	if (_actors.find(actorId) != _actors.end()){
		SLASSERT(_players.find(_actors[actorId]) != _players.end(), "wtf");
		Player& player = _players[_actors[actorId]];
		SLASSERT(player.state == GATE_STATE_BINDING && actorId == player.selectActorId, "wtf");

		if (errCode == ErrorCode::ERROR_NO_ERROR){
			player.state = GATE_STATE_ONLINE;
			player.lastActorId = actorId;

			//update DB


		}
		else{
			reset(pKernel, _actors[actorId], GATE_STATE_ROLELOADED);

			IBStream<128> buf;
			buf << errCode;
			sendToClient(pKernel, player.agentId, ServerMsgID::SERVER_MSG_SELECT_ROLE_RSP, buf.out());
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

	int64 accountId = 0;
	bool success = _cacheDB->readByIndex("account", [&](sl::api::IKernel* pKernel, ICacheDBReader* reader){
		reader->readColumn("accountId");
	}, [&accountId](sl::api::IKernel* pKernel, ICacheDBReadResult* result){
		if (result->count() == 1)
			accountId = result->getInt64(0, 0);
	}, accountName);

	SLASSERT(success, "read cacheDB failed");
	if (!accountId){
		accountId = (int64)_IdMgr->allocID();
		success = _cacheDB->write("account", [&](sl::api::IKernel* pKernel, ICacheDBContext* context){
			context->writeString("username", accountName);
		}, 1, accountId);
		SLASSERT(success, "write db failed");
	}
	SLASSERT(accountId, "can't get accountID");

	player.accountId = accountId;
	player.state = GATE_STATE_AUTHENING;

	IArgs<3, 128> inArgs;
	inArgs << player.agentId << player.accountId;
	inArgs.fix();

	_harbor->send(NodeType::ACCOUNT, 1, NodeProtocol::GATE_MSG_BIND_ACCOUNT_REQ, inArgs.out());
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

void Gate::onClientCreateRoleReq(sl::api::IKernel* pKernel, const int64 id, const OBStream& args){
	SLASSERT(_players.find(id) != _players.end(), "where is agent?");
	Player& player = _players[id];
	if (player.state == GATE_STATE_ROLELOADED){
		if (player.roles.size() >= _maxRoleNum){
			IBStream<128> buf;
			buf << ErrorCode::ERROR_TOO_MUCH_ROLE;
			sendToClient(pKernel, id, ServerMsgID::SERVER_MSG_CREATE_ROLE_RSP, buf.out());
			return;
		}

		int64 actorId = _IdMgr->allocID();
		IRole* role = _roleMgr->createRole(player.accountId, actorId, args);
		if (role){
			player.roles.push_back({ actorId, role });

			sl::IBStream<128> rsp;
			rsp << ErrorCode::ERROR_NO_ERROR;
			role->pack();
			sendToClient(pKernel, id, ServerMsgID::SERVER_MSG_CREATE_ROLE_RSP, rsp.out());
		}
		else{
			IBStream<128> buf;
			buf << ErrorCode::ERROR_CREATE_ROLE_FAILED;
			sendToClient(pKernel, id, ServerMsgID::SERVER_MSG_CREATE_ROLE_RSP, buf.out());
			return;
		}
	}
}

void Gate::test(){
	/*auto dbCall = CREATE_DB_CALL(_db, 0, 0);
	dbCall->insert("user", [&](sl::api::IKernel* pKernel, IDBInsertParamAdder* adder){
		adder->AddColumn("id", 877777888888);
		adder->AddColumn("name", "ddc");
	}, nullptr);*/
}