#include "Agent.h"
#include "AgentSession.h"
#include "slxml_reader.h"
#include "slstring_utils.h"

sl::SLPool<AgentSession> AgentSessionServer::s_pool;
sl::api::ITcpSession* AgentSessionServer::mallocTcpSession(sl::api::IKernel* pKernel){
	return CREATE_FROM_POOL(s_pool, _agent);
}

bool Agent::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	_agentPort = 0;
	return true;
}

bool Agent::launched(sl::api::IKernel * pKernel){
	_agentServer = NEW AgentSessionServer(this);
	if (!_agentServer){
		SLASSERT(false, "wtf");
		return false;
	}

	if (!pKernel->startTcpServer(_agentServer, "0.0.0.0", _agentPort)){
		SLASSERT(false, "wtf");
		TRACE_LOG("start agent server[%s:%d] failed", "0.0.0.0", _agentPort);
		return false;
	}
	else{
		TRACE_LOG("start agent server[%s:%d] success", "0.0.0.0", _agentPort);
	}
	
	return true;
}

bool Agent::destory(sl::api::IKernel * pKernel){
	if (_agentServer)
		DEL _agentServer;
	_agentServer = nullptr;
	
	_listener = nullptr;
	_agentNextId = 0;
	
	DEL this;
	return true;
}

int64 Agent::onOpen(AgentSession* pSession){
	if (_agentNextId <= 0)
		_agentNextId = 1;
	
	int64 ret = _agentNextId++;
	_agentSessions[ret] = pSession;

    printf("new agent connected.............\n");
	
	if (_listener)
		_listener->onAgentOpen(_kernel, ret);

	return ret;
}

int32 Agent::onRecv(int64 id, const char* pContext, const int32 size){
	SLASSERT(_agentSessions.find(id) != _agentSessions.end(), "where is agent %lld", id);
	if (_listener == nullptr)
		return 0;

	return _listener->onAgentRecv(_kernel, id, pContext, size);
    return 0;
}

void Agent::onClose(int64 id){
	SLASSERT(_agentSessions.find(id) != _agentSessions.end(), "where is agent %lld", id);
	_agentSessions.erase(id);
	
	if (_listener)
		_listener->onAgentClose(_kernel, id);
}

void Agent::setListener(IAgentListener* pListener){
	_listener = pListener;
}

void Agent::send(const int64 id, const void* pBuf, const int32 size){
	auto itor = _agentSessions.find(id);
	if (itor == _agentSessions.end()){
		SLASSERT(false, "where is agent %lld", id);
		return;
	}

	itor->second->send(pBuf, size);
}

void Agent::kick(const int64 id){
	auto itor = _agentSessions.find(id);
	if (itor == _agentSessions.end()){
		SLASSERT(false, "where is agent %lld", id);
		return;
	}

	itor->second->close();
}



