#include "sllistener.h"
#include "slnet_module.h"
namespace sl
{
CSLListener::CSLListener()
	:m_pNetworkInterface(nullptr),
	m_pSessionFactory(nullptr),
	m_dwRecvBufSize(0),
	m_dwSendBufSize(0),
	m_bStart(false)
{}

CSLListener::~CSLListener(){}

void CSLListener::setSessionFactory(ISLSessionFactory* poSessionFactory)
{
	m_pSessionFactory = poSessionFactory;
}

void CSLListener::setBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize)
{
	m_dwRecvBufSize = dwRecvBufSize;
	m_dwSendBufSize = dwSendBufSize;
}

bool CSLListener::start(const char* pszIP, uint16 wPort, bool bReUseAddr /* = true */)
{
	if(m_bStart)
	{
		return true;
	}

	if(nullptr == m_pSessionFactory)
	{
		return false;
	}

	m_pNetworkInterface = new NetworkInterface(CSLNetModule::getSingletonPtr()->getEventDispatcher(),
		wPort, wPort, pszIP);
	if( nullptr == m_pNetworkInterface)
	{
		return false;
	}
	return true;
}

bool CSLListener::stop()
{
	if(!m_bStart)
	{
		return true;
	}

	if(m_pNetworkInterface != nullptr)
	{
		m_pNetworkInterface->stop();
		delete m_pNetworkInterface;
		m_pNetworkInterface = nullptr;
	}

	m_bStart = false;

	return true;
}

void CSLListener::release()
{
	delete this;
}

}