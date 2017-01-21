#ifndef __SL_AGENT_SESSION_H__
#define __SL_AGENT_SESSION_H__
#include "slikernel.h"
#include "slobjectpool.h"

class Agent;
class AgentSession: public sl::api::ITcpSession
{
public:
	AgentSession();
	AgentSession(Agent* pAgent);
	~AgentSession();

	virtual int32 onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen);
	virtual void onConnected(sl::api::IKernel* pKernel);
	virtual void onDisconnect(sl::api::IKernel* pKernel);
private:
	int64	m_id;
	Agent*	m_agent;
};

CREATE_OBJECT_POOL(AgentSession);
#endif