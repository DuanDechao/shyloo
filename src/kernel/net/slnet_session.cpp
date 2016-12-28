#include "slnet_session.h"
#include "slkernel.h"
namespace sl
{
namespace core
{
NetSession::NetSession(ITcpSession* pTcpSession)
	:m_pTcpSession(pTcpSession),
	 m_pChannel(NULL)
{}

NetSession::~NetSession()
{
	//m_pTcpSession->close();
}

void NetSession::setChannel(ISLChannel* pChannel)
{
	m_pChannel = pChannel;
}

void NetSession::release()
{
	RELEASE_POOL_OBJECT(NetSession, this);
}

void NetSession::onRecv(const char* pBuf, uint32 dwLen)
{
	m_pTcpSession->onRecv(core::Kernel::getSingletonPtr(), pBuf, dwLen);
}

void NetSession::onEstablish()
{
	m_pTcpSession->onConnected(core::Kernel::getSingletonPtr());
}

void NetSession::onTerminate()
{
	m_pTcpSession->onDisconnect(core::Kernel::getSingletonPtr());
}

void NetSession::send(const void* pContext, int dwLen)
{
	m_pChannel->send((const char*)pContext, dwLen);
}

void NetSession::close()
{
	return m_pChannel->disconnect();
}

ISLSession* ServerSessionFactory::createSession(ISLChannel* poChannel)
{
	if(NULL == m_pServer)
		return NULL;

	ITcpSession* pTcpSession = m_pServer->mallocTcpSession(core::Kernel::getSingletonPtr());
	if(NULL == pTcpSession)
	{
		SLASSERT(false, "wtf");
		return NULL;
	}
	NetSession* pNetSession = CREATE_POOL_OBJECT(NetSession);
	if(NULL == pNetSession)
	{
		SLASSERT(false, "wtf");
		return NULL;
	}

	pNetSession->setChannel(poChannel);
	pNetSession->setTcpSession(pTcpSession);
	return pNetSession;
}

}
}