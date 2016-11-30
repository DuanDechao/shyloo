#include "slconnector.h"
#include "slnet_module.h"
#include "sladdress.h"
#include "sltcp_packet_receiver.h"

namespace sl
{
namespace network
{
CSLConnector::CSLConnector()
	:m_pSession(nullptr),
	 m_pSvrChannel(nullptr),
	 m_bConnected(false)
{
	m_pEventDispatcher = CSLNetModule::getSingletonPtr()->getEventDispatcher();
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
	if(m_bConnected)
		return false;

	m_pEndPoint = EndPoint::createPoolObject();

	Address addr;
	Address::string2ip(pszIP, addr.m_ip);
	addr.m_port = wPort;
	m_pEndPoint->addr(addr);

	if(m_pEndPoint->connect() == -1){
		EndPoint::reclaimPoolObject(m_pEndPoint);
		return false;
	}

	m_pSvrChannel = Channel::createPoolObject();
	m_pSvrChannel->setEndPoint(m_pEndPoint);
	
	TCPPacketReceiver* pTcpPacketReceiver = TCPPacketReceiver::createPoolObject();
	//m_pEventDispatcher->registerReadFileDescriptor((int32)*m_pEndPoint, )
}

}
}