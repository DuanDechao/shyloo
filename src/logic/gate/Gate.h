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
#include <list>
#include "slsingleton.h"
using namespace sl;

class IIdMgr;
class ICacheDB;

class IGateMessageHandler{
public:
	virtual ~IGateMessageHandler(){}
	virtual void DealNodeMessage(sl::api::IKernel*, const int64, const char* pContext, const int32 size) = 0;
};

class Gate :public IGate, public IAgentListener, public INodeListener, public sl::api::ITimer, public SLHolder<Gate>{
	enum {
		GATE_STATE_NONE = 0,
		GATE_STATE_AUTHENING,
		GATE_STATE_BINDING,
		GATE_STATE_ONLINE,
	};

/*	struct Role{
		int64 actorId;
		IRole* role;
	};
*/
	struct Session{
		int64 agentId;
	//	int64 selectActorId;
		int64 accountId;
//        uint64 proxyId;
		//int32 logic;
		//int64 lastActorId;
		int8 state;

		//std::list<Role> roles;
	};

public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void onAgentOpen(sl::api::IKernel* pKernel, const int64 id);
	virtual int32 onAgentRecv(sl::api::IKernel* pKernel, const int64 id, const void* context, const int32 size);
	virtual void onAgentClose(sl::api::IKernel* pKernel, const int64 id);

	virtual void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port);
	virtual void onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId);

	virtual void rgsGateMessageHandler(int32 messageId, const GATE_CB& handler, const char* debug);
	virtual void rgsGateArgsMessageHandler(int32 messageId, const GATE_ARGS_CB& handler, const char* debug);
	virtual void rgsGateListener(IGateListener* listener);

    virtual void sendMsgToAgent(const int64 agentId, const void* context, const int32 size, const int32 delay = 0);
    virtual void setGateOnline(const int64 agentId);
    //virtual void bindProxy(const int64 proxyId, const int64 agentId);

	//void onSceneMgrDistributeLogic(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onAccountBindAccountAck(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onAccountKickFromAccount(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	//void onLogicBindPlayerAck(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onBalanceSyncLoginTicket(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	//void onLogicTransforToAgent(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OBStream& args);
	//void onLogicBrocastToAgents(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OBStream& args);
	//void onLogicBrocastToAllAgents(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OBStream& args);

	void transMsgToLogic(sl::api::IKernel* pKernel, const int64 id, const void* pContext, const int32 size);

	void onClientLoginReq(sl::api::IKernel* pKernel, const int64 id, const OBStream& args);

	//void onClientSelectRoleReq(sl::api::IKernel* pKernel, const int64 id, const OBStream& args);
	//void onClientCreateRoleReq(sl::api::IKernel* pKernel, const int64 id, const OBStream& args);

//	void test();

	void broadcast(const void* context, const int32 size);
	void send(int64 actorId, const void* context, const int32 size);

	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick){}

private:
	void reset(sl::api::IKernel* pKernel, int64 id, int8 state);
	void sendToClient(sl::api::IKernel* pKernel, const int64 id, const int32 msgId, const OBStream& buf);
	void sendLoginAck(sl::api::IKernel* pKernel, Session& session, int32 errCode);

	/*inline Proxy* findProxyByProxyId(const int64 proxyId){
		auto itor = _proxyToAgentMap.find(proxyId);
		if (itor != _proxyToAgentMap.end()){
			SLASSERT(_proxyToAgentMap.find(itor->second) != _proxyToAgentMap.end(), "wtf");
			return &_proxys[itor->second];
		}
		else{
			return nullptr;
		}
	}*/

	inline Session* findSessionByAgentId(const int64 agentId){
		auto itor = _sessions.find(agentId);
		if (itor != _sessions.end())
			return &(itor->second);
		else
			return nullptr;
	}

	inline void forEach(const function<void(Session& session)>& func){
		auto itor = _sessions.begin();
		auto itorEnd = _sessions.end();
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
//	IRoleMgr*	_roleMgr;
	
	std::unordered_map<int64, Session> _sessions;
	//std::unordered_map<int64, int64> _proxys;
	//std::unordered_map<int32, std::unordered_set<int64>> _logicPlayers;
	std::unordered_map<int32, IGateMessageHandler*> _gateProtos;
	std::unordered_map<int64, int64> _agentTickets;
    std::list<IGateListener*> _gateListeners;
	bool _isQueneOn;
};

#endif
