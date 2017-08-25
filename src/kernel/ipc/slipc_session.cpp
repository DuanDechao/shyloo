#include "slipc_session.h"
#include "slkernel.h"
#include "slipc_engine.h"

namespace sl{
namespace core{
#define SINGLE_RECV_SIZE 32768
sl::SLPool<IPCSession> IPCSession::s_pool;
IPCSession::IPCSession(ITcpSession* pTcpSession, shm::ISLShmQueue* shmQueue, int64 localId, int64 remoteId)
	:_tcpSession(pTcpSession),
	_shmQueue(shmQueue),
	_localId(localId),
	_remoteId(remoteId)
{
	_tcpSession->_pipe = this;
}

IPCSession::~IPCSession(){
	//m_pTcpSession->close();
}

void IPCSession::onRecv(const char* pBuf, uint32 dwLen){
	_tcpSession->onRecv(core::Kernel::getInstance(), pBuf, dwLen);
}

void IPCSession::onEstablish(){
	_tcpSession->onConnected(core::Kernel::getInstance());
}

void IPCSession::onTerminate(){
	_tcpSession->onDisconnect(core::Kernel::getInstance());
}

void IPCSession::send(const void* pContext, int dwLen){
	_shmQueue->putData(pContext, dwLen);
}

void IPCSession::close(){
	((IPCEngine*)IPCEngine::getInstance())->close(_remoteId, _localId);
}

const char* IPCSession::getRemoteIP(){
	return Kernel::getInstance()->getInternetIp();
}

int32 IPCSession::procRecv(){
	char temp[SINGLE_RECV_SIZE];
	while (true){
		const char* data = _shmQueue->peekData(temp, sizeof(int32)* 2);
		if (data == nullptr)
			break;

		int32 msgSize = *(int32*)(data + sizeof(int32));
		if (msgSize > SINGLE_RECV_SIZE){
			SLASSERT(false, "wtf");
			close();
			return -1;
		}
		data = _shmQueue->peekData(temp, msgSize);
		if (data == nullptr)
			break; 

		onRecv(data, msgSize);
		_shmQueue->skip(msgSize);
	}

	return 0;
}

IPCSession* IPCSessionFactory::createSession(shm::ISLShmQueue* queue, int64 localId, int64 remoteId){
	if (NULL ==_server)
		return NULL;

	ITcpSession* pTcpSession = _server->mallocTcpSession(core::Kernel::getInstance());
	if (NULL == pTcpSession){
		SLASSERT(false, "wtf");
		return NULL;
	}
	IPCSession* pIPCSession = IPCSession::create(pTcpSession, queue, localId, remoteId);
	if (NULL == pIPCSession){
		SLASSERT(false, "wtf");
		return NULL;
	}

	return pIPCSession;
}


}
}