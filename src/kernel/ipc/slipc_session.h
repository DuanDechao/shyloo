#ifndef SL_KERNEL_IPC_SESSION_H
#define SL_KERNEL_IPC_SESSION_H
#include "slikernel.h"
#include "slobjectpool.h"
#include "slshm.h"
namespace sl{
namespace core{
using namespace api;
class IPCSession: public api::IPipe{
	struct ShmMsgBuf{
		char buf[4096];
		int32 currSize;
		int32 msgSize;
	};
public:
	IPCSession(){}
	IPCSession(ITcpSession* pTcpSession, shm::ISLShmQueue* shmQueue, int64 localId, int64 remoteId);
	virtual ~IPCSession();

	virtual void release();
	virtual void onRecv(const char* pBuf, uint32 dwLen);
	virtual void onEstablish(void);
	virtual void onTerminate();
	virtual const char* getRemoteIP() { return ""; }

	virtual void send(const void* pContext, int dwLen);
	virtual void close();

	virtual void setTcpSession(ITcpSession* pTcpSession) {m_pTcpSession = pTcpSession;}

	int32 procRecv();

	inline int64 getLocalId()const  { return m_localId; }
	inline int64 getRemoteId()const  { return m_remoteId; }
	inline int32 getBufSize() const { m_pShmQueue->getQueueSize(); }

private:
	ITcpSession*	m_pTcpSession;
	shm::ISLShmQueue*    m_pShmQueue;
	int64			m_localId;
	int64           m_remoteId;
	ShmMsgBuf       m_msgBuf;
	
};

class IPCSessionFactory{
public:
	IPCSessionFactory(ITcpServer* pServer) :m_pServer(pServer){}
	virtual ~IPCSessionFactory(){}
	virtual IPCSession* createSession(shm::ISLShmQueue* queue, int64 localId, int64 remoteId);
public:
	ITcpServer*		m_pServer;
};

CREATE_OBJECT_POOL(IPCSession);

}
}

#endif