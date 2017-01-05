#include "slnet_session.h"
#include "slkernel.h"
namespace sl
{
namespace core
{
int32 NetPacketParser::parsePacket(const char* pDataBuf, int32 len){
	if (!pDataBuf)
		return -1;
	
	if (len < 2 * sizeof(int32))
		return 0;

	int32 dwLen = *(int32*)(pDataBuf + sizeof(int32));
	ECHO_TRACE("recv len %d %d", len, dwLen);
	if (len >= dwLen)
		return dwLen;
	else
		return 0;
}

NetSession::NetSession(ITcpSession* pTcpSession)
	:m_pTcpSession(pTcpSession),
	 m_pChannel(NULL)
{
	m_pTcpSession->m_pPipe = this;
}

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
	m_pTcpSession->onRecv(core::Kernel::getInstance(), pBuf, dwLen);
}

void NetSession::onEstablish()
{
	m_pTcpSession->onConnected(core::Kernel::getInstance());
}

void NetSession::onTerminate()
{
	m_pTcpSession->onDisconnect(core::Kernel::getInstance());
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

	ITcpSession* pTcpSession = m_pServer->mallocTcpSession(core::Kernel::getInstance());
	if(NULL == pTcpSession)
	{
		SLASSERT(false, "wtf");
		return NULL;
	}
	NetSession* pNetSession = CREATE_POOL_OBJECT(NetSession, pTcpSession);
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