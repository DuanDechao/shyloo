#include "TelnetSession.h"
#include "TelnetServer.h"
sl::SLPool<TelnetSession> TelnetSession::s_pool;
int32 TelnetSession::onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen){
	return dwLen;
}

void TelnetSession::send(const void* pContext, const int32 size){
	ITcpSession::send(pContext, size); 
}

void TelnetSession::onConnected(sl::api::IKernel* pKernel){
}

void TelnetSession::onDisconnect(sl::api::IKernel* pKernel){
	release();
}
