#ifndef __SL_CORE_AGENT_H__
#define __SL_CORE_AGENT_H__
#include "IAgent.h"
#include <unordered_map>
class IHarbor;
class AgentSession;
class Agent;
class AgentSessionServer : public sl::api::ITcpServer{
public:
	AgentSessionServer(Agent* pAgent) :m_agent(pAgent){}
	virtual ~AgentSessionServer(){}
	virtual sl::api::ITcpSession* mallocTcpSession(sl::api::IKernel* pKernel);

private:
	Agent* m_agent;
};

class Agent : public IAgent
{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void setListener(IAgentListener* pListener);

	int32 onRecv(int64 id, const char* pContext, const int32 size);
	int64 onOpen(AgentSession* pSession);
	void onClose(int64 id);

	virtual void send(const int64 id, const void* pBuf, const int32 size);
	virtual void kick(const int64 id);

private:
	static sl::api::IKernel*    s_kernel;
	static IHarbor*				s_harbor;
	static int64				s_agentNextId;
	static IAgentListener*		s_listener;
	AgentSessionServer*			m_agentServer;
	static std::unordered_map<int64, AgentSession*> s_agentSessions;
};
#endif