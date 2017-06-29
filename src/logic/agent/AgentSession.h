#ifndef __SL_AGENT_SESSION_H__
#define __SL_AGENT_SESSION_H__
#include "slikernel.h"
#include "slpool.h"

class Agent;
class AgentSession: public sl::api::ITcpSession{
public:
	AgentSession(Agent* pAgent):_id(0), _agent(pAgent){}
	virtual ~AgentSession(){}

	virtual int32 onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen);
	virtual void onConnected(sl::api::IKernel* pKernel);
	virtual void onDisconnect(sl::api::IKernel* pKernel);

private:
	int64	_id;
	Agent*	_agent;
};

#endif