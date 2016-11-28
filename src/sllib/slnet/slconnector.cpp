#include "slconnector.h"
#include "slnet_module.h"
#include "slchannel.h"
#include ""
namespace sl
{
namespace network
{
CSLConnector::CSLConnector()
	:m_pNetworkInterface(nullptr),
	 m_pSession(nullptr),
	 m_dwRecvBufSize(0),
	 m_dwSendBufSize(0),
	 m_connEndPoint()
{}

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

bool CSLConnector::connect(const char* pszIp, uint16 wPort)
{
	m_pNetworkInterface = new NetworkInterface(CSLNetModule::getSingletonPtr()->getEventDispatcher());
	Channel* pChannel = Channel::createPoolObject();
	m_tcpPacketReceiver = new TCPPacketReceiver(m_connEndPoint, *m_pNetworkInterface);
	bool ret = pChannel->initialize(*m_pNetworkInterface, &m_connEndPoint, Channel::Traits::INTERNAL);
	if(!ret)
	{
		pChannel->destroy();
		Channel::reclaimPoolObject(pChannel);
		return false;
	}

	if(!m_pNetworkInterface->registerChannel(pChannel))
	{
		pChannel->destroy();
		Channel::reclaimPoolObject(pChannel);
		return false;
	}
	return m_pNetworkInterface->createConnectingSocket(pszIp, wPort, &m_connEndPoint, m_tcpPacketReceiver, m_dwRecvBufSize, m_dwSendBufSize);
}
}
}