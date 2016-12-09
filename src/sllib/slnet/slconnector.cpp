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
	 m_pSvrEndPoint(),
	 m_dwRecvBufSize(0),
	 m_dwSendBufSize(0)
{
	m_pNetworkInterface = CSLNetModule::getSingletonPtr()->getNetworkInterface();
}

CSLConnector::~CSLConnector()
{
}

void CSLConnector::setSession(ISLSession* pSession)
{
	m_pSession = pSession;
}

void CSLConnector::setBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize)
{
	m_dwRecvBufSize = dwRecvBufSize;
	m_dwSendBufSize = dwSendBufSize;
}

bool CSLConnector::connect(const char* pszIP, uint16 wPort)
{
	if(nullptr == m_pSession || nullptr == m_pNetworkInterface){
		SLASSERT(false, "wtf");
		return false;
	}
	return m_pNetworkInterface->createConnectingSocket(pszIP, wPort, &m_pSvrEndPoint, m_pSession, m_pPacketParser, m_dwRecvBufSize, m_dwSendBufSize);
	
}
void CSLConnector::release()
{
	if (m_pSvrEndPoint.good())
	{
		m_pNetworkInterface->deregisterSocket((int32)m_pSvrEndPoint);
		m_pSvrEndPoint.close();
	}
	m_dwRecvBufSize = 0;
	m_dwSendBufSize = 0;
	delete this;
}

void CSLConnector::setPacketParser(ISLPacketParser* poPacketParser){
	m_pPacketParser = poPacketParser;
}


}
}