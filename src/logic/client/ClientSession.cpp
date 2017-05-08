#include "ClientSession.h"
#include "Client.h"
ClientSession::ClientSession() :m_id(0), m_client(nullptr){}
ClientSession::ClientSession(Client* pClient) : m_id(0), m_client(pClient){}
ClientSession::~ClientSession(){
	m_id = 0;
	m_client = nullptr;
}

int32 ClientSession::onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen){
	return m_client->onRecv(m_id, pContext, dwLen);
}

void ClientSession::onConnected(sl::api::IKernel* pKernel){
	m_id = m_client->onOpen(this);
}

void ClientSession::onDisconnect(sl::api::IKernel* pKernel){
	m_client->onClose(m_id);
}

void ClientSession::onTime(sl::api::IKernel* pKernel, int64 timetick){
	SLASSERT(m_bConnect, "wtf");
	ECHO_TRACE("reconnect [%s:%d] ...!", m_ip.c_str(), m_port);
	if (pKernel->startTcpClient(this, m_ip.c_str(), m_port, m_client->sendSize(), m_client->recvSize())){
		pKernel->killTimer(this);
	}
}

void ClientSession::setConnect(const char* ip, const int32 port){
	m_ip = ip;
	m_port = port;
	m_bConnect = true;
}