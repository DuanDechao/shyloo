#ifndef __SL_CORE_AGENT_H__
#define __SL_CORE_AGENT_H__
#include "IAgent.h"
#include <unordered_map>
#include "slsingleton.h"
#include "slpool.h"
#include "AgentSession.h"

class IHarbor;
class Agent;
class AgentSessionServer : public sl::api::ITcpServer{
public:
	AgentSessionServer(Agent* pAgent) :_agent(pAgent){}
	virtual ~AgentSessionServer(){}
	virtual sl::api::ITcpSession* mallocTcpSession(sl::api::IKernel* pKernel);

	inline void recover(AgentSession* session){
		s_pool.recover(session);
	}

private:
	Agent* _agent;
	static sl::SLPool<AgentSession> s_pool;
};

class Agent : public IAgent, public sl::SLHolder<Agent>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void setListener(IAgentListener* pListener);

	int32 onRecv(int64 id, const char* pContext, const int32 size);
	int64 onOpen(AgentSession* pSession);
	void onClose(int64 id);

	inline void recover(AgentSession* session){
		_agentServer->recover(session);
	}

	virtual void send(const int64 id, const void* pBuf, const int32 size);
	virtual void kick(const int64 id);

private:
	sl::api::IKernel*   _kernel;
	IHarbor*			_harbor;
	int64				_agentNextId;
	IAgentListener*		_listener;
	AgentSessionServer* _agentServer;
	std::unordered_map<int64, AgentSession*> _agentSessions;
};
#endif