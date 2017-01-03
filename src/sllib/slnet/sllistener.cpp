#include "sllistener.h"
#include "slnet_module.h"
#include "sllistener_receiver.h"
namespace sl
{
CSLListener::CSLListener()
	:m_dwRecvBufSize(0),
	 m_dwSendBufSize(0),
	 m_pListenEndPoint(),
	 m_pNetworkInterface(nullptr),
	 m_pListenerReceiver(nullptr)
{
	m_pNetworkInterface = CSLNetModule::getSingletonPtr()->getNetworkInterface();
	m_pListenerReceiver = new ListenerReceiver(&m_pListenEndPoint, m_pNetworkInterface);
}

CSLListener::~CSLListener()
{
	if (nullptr != m_pListenerReceiver)
		delete m_pListenerReceiver;

	m_pListenerReceiver = nullptr;

	stop();
	m_dwRecvBufSize = 0;
	m_dwSendBufSize = 0;
}

void CSLListener::setSessionFactory(ISLSessionFactory* poSessionFactory)
{
	m_pListenerReceiver->setSessionFactory(poSessionFactory);
}

void CSLListener::setBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize)
{
	m_dwRecvBufSize = dwRecvBufSize;
	m_dwSendBufSize = dwSendBufSize;
}

bool CSLListener::start(const char* pszIP, uint16 wPort, bool bReUseAddr /* = true */)
{
	SLASSERT(m_pNetworkInterface, "wtf");
	if (m_pNetworkInterface == nullptr || m_pListenerReceiver == nullptr || 
		m_dwRecvBufSize <= 0 || m_dwSendBufSize <= 0){
		return false;
	}

	return m_pNetworkInterface->createListeningSocket(pszIP, wPort, &m_pListenEndPoint, m_pListenerReceiver,
		m_dwRecvBufSize, m_dwSendBufSize);
}

bool CSLListener::stop()
{
	if (m_pListenEndPoint.good())
	{
		m_pNetworkInterface->deregisterSocket((int32)m_pListenEndPoint);
		m_pListenEndPoint.close();
	}
	return true;
}

void CSLListener::setPacketParser(ISLPacketParser* poPacketParser)
{
	m_pListenerReceiver->setPacketParser(poPacketParser);
}

void CSLListener::release()
{
	DEL this;
}

}