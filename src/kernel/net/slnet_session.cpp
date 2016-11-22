#include "slnet_session.h"
#include "slkernel.h"
#include "slassert.h"
namespace sl
{
namespace network
{

static CObjectPool<NetSession> g_objPool("NetSession");
CObjectPool<NetSession>& NetSession::ObjPool()
{
	return g_objPool;
}

NetSession* NetSession::createPoolObject()
{
	return g_objPool.FetchObj();
}

void NetSession::reclaimPoolObject(NetSession* obj)
{
	g_objPool.ReleaseObj(obj);
}

void NetSession::destroyObjPool()
{
	g_objPool.Destroy();
}

size_t NetSession::getPoolObjectBytes()
{
	size_t bytes = sizeof(m_pChannel) + sizeof(m_pTcpSession);
	return bytes;
}

NetSession::SmartPoolObjectPtr NetSession::createSmartPoolObj()
{
	return SmartPoolObjectPtr(new SmartPoolObject<NetSession>(ObjPool().FetchObj(), g_objPool));
}

void NetSession::onReclaimObject()
{
	//this->clearState();
}

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
	NetSession::reclaimPoolObject(this);
}

void NetSession::onRecv(const char* pBuf, uint32 dwLen)
{
	m_pTcpSession->onRecv(core::Kernel::getSingletonPtr(), pBuf, dwLen);
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
		SL_ASSERT(false);
		return NULL;
	}
	NetSession* pNetSession = NetSession::createPoolObject();
	if(NULL == pNetSession)
	{
		SL_ASSERT(false);
		return NULL;
	}

	pNetSession->setChannel(poChannel);
	pNetSession->setTcpSession(pTcpSession);
	return pNetSession;
}

}
}