#include "AgentSession.h"
#include "Agent.h"
int32 AgentSession::onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen){
	return _agent->onRecv(_id, pContext, dwLen);
}

void AgentSession::onConnected(sl::api::IKernel* pKernel){
	_id = _agent->onOpen(this);
}

void AgentSession::onDisconnect(sl::api::IKernel* pKernel){
	_agent->onClose(_id);
	_agent->recover(this);
}