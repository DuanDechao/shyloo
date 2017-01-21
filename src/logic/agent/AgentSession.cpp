#include "AgentSession.h"
#include "Agent.h"
AgentSession::AgentSession() :m_id(0), m_agent(nullptr){}
AgentSession::AgentSession(Agent* pAgent) : m_id(0), m_agent(pAgent){}
AgentSession::~AgentSession(){
	m_id = 0;
	m_agent = nullptr;
}

int32 AgentSession::onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen){
	return m_agent->onRecv(m_id, pContext, dwLen);
}

void AgentSession::onConnected(sl::api::IKernel* pKernel){
	m_id = m_agent->onOpen(this);
}

void AgentSession::onDisconnect(sl::api::IKernel* pKernel){
	m_agent->onClose(m_id);
}