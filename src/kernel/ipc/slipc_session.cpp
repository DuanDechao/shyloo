#include "slipc_session.h"
#include "slkernel.h"
namespace sl{
namespace core{
IPCSession::IPCSession(ITcpSession* pTcpSession, shm::ISLShmQueue* shmQueue, int64 localId, int64 remoteId)
	:m_pTcpSession(pTcpSession),
	m_pShmQueue(shmQueue),
	m_localId(localId),
	m_remoteId(remoteId)
{
	m_pTcpSession->m_pPipe = this;
}

IPCSession::~IPCSession()
{
	//m_pTcpSession->close();
}

void IPCSession::release(){
	RELEASE_POOL_OBJECT(IPCSession, this);
}

void IPCSession::onRecv(const char* pBuf, uint32 dwLen){
	m_pTcpSession->onRecv(core::Kernel::getInstance(), pBuf, dwLen);
}

void IPCSession::onEstablish(){
	m_pTcpSession->onConnected(core::Kernel::getInstance());
}

void IPCSession::onTerminate(){
	m_pTcpSession->onDisconnect(core::Kernel::getInstance());
}

void IPCSession::send(const void* pContext, int dwLen){
	m_pShmQueue->putData(pContext, dwLen);
}

void IPCSession::close(){

}

const char* IPCSession::getRemoteIP(){
	return Kernel::getInstance()->getInternetIp();
}

int32 IPCSession::procRecv(){
	char temp[10240];
	while (true){
		const char* data = m_pShmQueue->peekData(temp, sizeof(int32)* 2);
		if (data == nullptr)
			break;

		int32 msgSize = *(int32*)(data + sizeof(int32));
		data = m_pShmQueue->peekData(temp, msgSize);
		if (data == nullptr)
			break;

		onRecv(data, msgSize);
		m_pShmQueue->skip(msgSize);
	}

	return 0;
}

IPCSession* IPCSessionFactory::createSession(shm::ISLShmQueue* queue, int64 localId, int64 remoteId){
	if (NULL == m_pServer)
		return NULL;

	ITcpSession* pTcpSession = m_pServer->mallocTcpSession(core::Kernel::getInstance());
	if (NULL == pTcpSession){
		SLASSERT(false, "wtf");
		return NULL;
	}
	IPCSession* pIPCSession = CREATE_POOL_OBJECT(IPCSession, pTcpSession, queue, localId, remoteId);
	if (NULL == pIPCSession){
		SLASSERT(false, "wtf");
		return NULL;
	}

	return pIPCSession;
}


}
}