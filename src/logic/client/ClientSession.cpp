#include "ClientSession.h"
#include "Client.h"
int32 ClientSession::onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen){
	return _client->onRecv(pContext, dwLen);
}

void ClientSession::onConnected(sl::api::IKernel* pKernel){
	TRACE_LOG("client session connected");
	_client->onOpen(this);
}

void ClientSession::onDisconnect(sl::api::IKernel* pKernel){
	_client->onClose();
}

void ClientSession::onTime(sl::api::IKernel* pKernel, int64 timetick){
	TRACE_LOG("reconnect [%s:%d] ...!", _ip.c_str(), _port);
	if (pKernel->startTcpClient(this, _ip.c_str(), _port, _client->sendSize(), _client->recvSize())){
		pKernel->killTimer(this);
	}
}

