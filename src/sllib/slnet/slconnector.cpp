#include "slconnector.h"
#include "slnet_module.h"
#include "sladdress.h"
#include "sltcp_packet_sender.h"

namespace sl
{
namespace network
{
CSLConnector::CSLConnector()
	:m_pSession(nullptr),
	 m_dwRecvBufSize(0),
	 m_dwSendBufSize(0),
	 m_pszIP(nullptr),
	 m_wPort(0)
{
	m_pNetworkInterface = CSLNetModule::getSingletonPtr()->getNetworkInterface();
}

CSLConnector::~CSLConnector(){
	m_pSession = NULL;
	m_pNetworkInterface = NULL;
	m_pPacketParser = NULL;
	m_dwRecvBufSize = 0;
	m_dwSendBufSize = 0;
	m_pszIP = NULL;
	m_wPort = 0;
}

void CSLConnector::setSession(ISLSession* pSession){
	m_pSession = pSession;
}

void CSLConnector::setBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize)
{
	m_dwRecvBufSize = dwRecvBufSize;
	m_dwSendBufSize = dwSendBufSize;
}

bool CSLConnector::connect(const char* pszIP, uint16 wPort)
{
	if (nullptr == m_pSession || nullptr == m_pNetworkInterface || m_pPacketParser == nullptr
		|| m_dwRecvBufSize <= 0 || m_dwSendBufSize <= 0){
		SLASSERT(false, "wtf");
		return false;
	}
	m_pszIP = pszIP;
	m_wPort = wPort;
	return m_pNetworkInterface->createConnectingSocket(pszIP, wPort, m_pSession, m_pPacketParser, m_dwRecvBufSize, m_dwSendBufSize);
}

bool CSLConnector::reConnect()
{
	if (m_pszIP == nullptr || m_wPort == 0)
		return false;

	return connect(m_pszIP, m_wPort);
}
void CSLConnector::release(){
	m_dwRecvBufSize = 0;
	m_dwSendBufSize = 0;

	Address addr(m_pszIP, m_wPort);
	Channel* pSvrChannel = m_pNetworkInterface->findChannel(addr);
	if (pSvrChannel != NULL){
		pSvrChannel->disconnect();
	}

	DEL this;
}

void CSLConnector::setPacketParser(ISLPacketParser* poPacketParser){
	m_pPacketParser = poPacketParser;
}

}
}