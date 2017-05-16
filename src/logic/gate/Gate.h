#ifndef __SL_FRAMEWORK_GATE_H__
#define __SL_FRAMEWORK_GATE_H__
#include "slikernel.h"
#include "slimodule.h"
#include "IGate.h"
#include "IAgent.h"
#include "IHarbor.h"
#include "slbinary_stream.h"
#include <unordered_map>
#include <unordered_set>
#include "slsingleton.h"
using namespace sl;

class IIdMgr;
class IRoleMgr;
class IRole;
class ICacheDB;
class Gate :public IGate, public IAgentListener, public INodeListener, public SLHolder<Gate>{
	enum {
		GATE_STATE_NONE = 0,
		GATE_STATE_AUTHENING,
		GATE_STATE_ROLELOADED,
		GATE_STATE_DISTRIBUTE,
		GATE_STATE_BINDING,
		GATE_STATE_ONLINE,
	};

	struct Role{
		int64 actorId;
		IRole* role;
	};

	struct Player{
		int64 agentId;
		int64 selectActorId;
		int64 accountId;
		int32 logic;
		int64 lastActorId;
		int8 state;

		std::list<Role> roles;
	};

	typedef void(Gate::*agent_cb)(sl::api::IKernel* pKernel, const int64 id, const char* pContext, const int32 size);
	typedef void(Gate::*agent_args_cb)(sl::api::IKernel* pKernel, const int64 id, const OBStream& args);

public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void onAgentOpen(sl::api::IKernel* pKernel, const int64 id);
	virtual int32 onAgentRecv(sl::api::IKernel* pKernel, const int64 id, const void* context, const int32 size);
	virtual void onAgentClose(sl::api::IKernel* pKernel, const int64 id);

	virtual void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){}
	virtual void onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId);

	void onSceneMgrDistributeLogic(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onAccountBindAccountAck(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onAccountKickFromAccount(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onLogicBindPlayerAck(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onLogicTransforToAgent(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OBStream& args);
	void onLogicBrocastToAgents(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OBStream& args);
	void onLogicBrocastToAllAgents(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OBStream& args);

	void rgsAgentMessageHandler(int32 messageId, agent_args_cb handler);
	void transMsgToLogic(sl::api::IKernel* pKernel, const int64 id, const void* pContext, const int32 size);

	void onClientLoginReq(sl::api::IKernel* pKernel, const int64 id, const OBStream& args);

	void onClientSelectRoleReq(sl::api::IKernel* pKernel, const int64 id, const OBStream& args);
	void onClientCreateRoleReq(sl::api::IKernel* pKernel, const int64 id, const OBStream& args);

	void test();

	void broadcast(const void* context, const int32 size);
	void send(int64 actorId, const void* context, const int32 size);

private:
	void reset(sl::api::IKernel* pKernel, int64 id, int8 state);
	void sendToClient(sl::api::IKernel* pKernel, const int64 id, const int32 msgId, const OBStream& buf);

	inline Player* findPlayerByActorId(const int64 actorId){
		auto itor = _actors.find(actorId);
		if (itor != _actors.end()){
			SLASSERT(_players.find(itor->second) != _players.end(), "wtf");
			return &_players[itor->second];
		}
		else{
			return nullptr;
		}
	}

	inline Player* findPlayerByAgentId(const int64 agentId){
		auto itor = _players.find(agentId);
		if (itor != _players.end())
			return &(itor->second);
		else
			return nullptr;
	}

	inline void forEach(const function<void(Player& player)>& func){
		auto itor = _players.begin();
		auto itorEnd = _players.end();
		for (; itor != itorEnd; ++itor){
			func(itor->second);
		}
	}

private:
	sl::api::IKernel* _kernel;
	Gate*		_self;
	IHarbor*	_harbor;
	IAgent*		_agent;
	ICacheDB*	_cacheDB;
	IIdMgr*		_IdMgr;
	IRoleMgr*	_roleMgr;
	
	int32		_maxRoleNum;
	std::unordered_map<int64, Player> _players;
	std::unordered_map<int64, int64> _actors;
	std::unordered_map<int32, std::unordered_set<int64>> _logicPlayers;
	std::unordered_map<int32, agent_args_cb> _gateProtos;
};
#endif