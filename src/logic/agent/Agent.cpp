#include "Agent.h"
#include "AgentSession.h"
#include "slxml_reader.h"
#include "slstring_utils.h"
sl::api::IKernel* Agent::s_kernel = nullptr;
IHarbor* Agent::s_harbor = nullptr;
int64 Agent::s_agentNextId = 0;
IAgentListener*	Agent::s_listener = nullptr;
std::unordered_map<int64, AgentSession*> Agent::s_agentSessions;

sl::api::ITcpSession* AgentSessionServer::mallocTcpSession(sl::api::IKernel* pKernel){
	return CREATE_POOL_OBJECT(AgentSession, m_agent);
}
bool Agent::initialize(sl::api::IKernel * pKernel){
	s_kernel = pKernel;
	return true;
}

bool Agent::launched(sl::api::IKernel * pKernel){
	s_harbor = (IHarbor*)pKernel->findModule("Harbor");
	SLASSERT(s_harbor, "not find module harbor");

	sl::XmlReader server_conf;
	if (!server_conf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "can not load core file %s", pKernel->getCoreFile());
		return false;
	}
	const sl::ISLXmlNode& agentConf = server_conf.root()["agent"][0];
	int32 agentRecvSize = agentConf.getAttributeInt32("recv");
	int32 agentSendSize = agentConf.getAttributeInt32("send");
	int32 agentPort = sl::CStringUtils::StringAsInt32(pKernel->getCmdArg("agent"));
	
	m_agentServer = NEW AgentSessionServer(this);
	if (!m_agentServer){
		SLASSERT(false, "wtf");
		return false;
	}

	if (!pKernel->startTcpServer(m_agentServer, "0.0.0.0", agentPort, agentSendSize, agentRecvSize)){
		SLASSERT(false, "wtf");
		return false;
	}
	return true;
}

bool Agent::destory(sl::api::IKernel * pKernel){
	if (m_agentServer)
		DEL m_agentServer;
	m_agentServer = nullptr;
	
	s_listener = nullptr;
	s_agentNextId = 0;
	s_harbor = nullptr;
	
	DEL this;
	return true;
}

int64 Agent::onOpen(AgentSession* pSession){
	if (s_agentNextId <= 0)
		s_agentNextId = 1;
	
	int64 ret = s_agentNextId++;
	s_agentSessions[ret] = pSession;
	
	if (s_listener)
		s_listener->onAgentOpen(s_kernel, ret);

	return ret;
}

int32 Agent::onRecv(int64 id, const char* pContext, const int32 size){
	SLASSERT(s_agentSessions.find(id) != s_agentSessions.end(), "where is agent %lld", id);
	if (s_listener == nullptr)
		return 0;

	return s_listener->onAgentRecv(s_kernel, id, pContext, size);
}

void Agent::onClose(int64 id){
	SLASSERT(s_agentSessions.find(id) != s_agentSessions.end(), "where is agent %lld", id);
	s_agentSessions.erase(id);
	
	if (s_listener)
		s_listener->onAgentClose(s_kernel, id);
}

void Agent::setListener(IAgentListener* pListener){
	s_listener = pListener;
}

void Agent::send(const int64 id, const void* pBuf, const int32 size){
	auto itor = s_agentSessions.find(id);
	if (itor == s_agentSessions.end()){
		SLASSERT(false, "where is agent %lld", id);
		return;
	}

	itor->second->send(pBuf, size);
}

void Agent::kick(const int64 id){
	auto itor = s_agentSessions.find(id);
	if (itor == s_agentSessions.end()){
		SLASSERT(false, "where is agent %lld", id);
		return;
	}

	itor->second->close();
}



