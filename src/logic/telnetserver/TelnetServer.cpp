#include "TelnetServer.h"
#include "slargs.h"
#include "IHarbor.h"
#include "IDebugHelper.h"

sl::api::ITcpSession* TelnetSessionTcpServer::mallocTcpSession(sl::api::IKernel* pKernel){
	return TelnetSession::create(_telnetServer);
}

bool TelnetServer::initialize(sl::api::IKernel * pKernel){
	_pKernel = pKernel;
	_port = 7123;
    return true;
}

bool TelnetServer::launched(sl::api::IKernel * pKernel){
	_pTelnetServer = NEW TelnetSessionTcpServer(this);
	if (!_pTelnetServer){
		SLASSERT(false, "wtf");
		return false;
	}

	startListening(pKernel);
	return true; 
}

bool TelnetServer::destory(sl::api::IKernel * pKernel){
	if (_pTelnetServer)
		DEL _pTelnetServer;
	_pTelnetServer = nullptr;

	DEL this;
	return true;
}

void TelnetServer::startListening(sl::api::IKernel* pKernel){
	if (pKernel->startTelnetServer(_pTelnetServer, "0.0.0.0", _port)){
		TRACE_LOG("start telnet server[%s:%d] success", "0.0.0.0", _port);
	}
	else{
		TRACE_LOG("start telnet server[%s:%d] failed", "0.0.0.0", _port);
	}
}


void TelnetServer::rgsTelnetHandler(const char* handlerName, ITelnetHandler* handler){
	if(_telnetHandlers.find(handlerName) != _telnetHandlers.end()){
		SLASSERT(false, "wtf");
		return;
	}
	_telnetHandlers[handlerName] = handler;
}

ITelnetHandler* TelnetServer::findTelnetHandler(const char* handlerName){
	auto itor = _telnetHandlers.find(handlerName);
	if(itor == _telnetHandlers.end())
		return NULL;

	return itor->second;
}

