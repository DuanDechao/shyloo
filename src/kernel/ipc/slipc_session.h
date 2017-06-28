#ifndef SL_KERNEL_IPC_SESSION_H
#define SL_KERNEL_IPC_SESSION_H
#include "slikernel.h"
#include "slshm.h"
#include "slpool.h"

namespace sl{
namespace core{
using namespace api;
class IPCSession: public api::IPipe{
public:
	inline static IPCSession* create(ITcpSession* tcpSession, shm::ISLShmQueue* shmQueue, int64 localId, int64 remoteId){
		return CREATE_FROM_POOL(s_pool, tcpSession, shmQueue, localId, remoteId);
	}

	inline void release(){
		s_pool.recover(this);
	}

	virtual void onRecv(const char* pBuf, uint32 dwLen);
	virtual void onEstablish(void);
	virtual void onTerminate();
	virtual const char* getRemoteIP();
	virtual void adjustSendBuffSize(const int32 size) {}
	virtual void adjustRecvBuffSize(const int32 size) {}

	virtual void send(const void* pContext, int dwLen);
	virtual void close();

	virtual void setTcpSession(ITcpSession* pTcpSession) { _tcpSession = pTcpSession; }

	int32 procRecv();

	inline int64 getLocalId()const  { return _localId; }
	inline int64 getRemoteId()const  { return _remoteId; }

private:
	friend sl::SLPool<IPCSession>;
	IPCSession(ITcpSession* pTcpSession, shm::ISLShmQueue* shmQueue, int64 localId, int64 remoteId);
	virtual ~IPCSession();

private:
	ITcpSession*			_tcpSession;
	shm::ISLShmQueue*		_shmQueue;
	int64					_localId;
	int64					_remoteId;
	static sl::SLPool<IPCSession> s_pool;
	
};

class IPCSessionFactory{
public:
	IPCSessionFactory(ITcpServer* pServer) :_server(pServer){}
	virtual ~IPCSessionFactory(){}
	virtual IPCSession* createSession(shm::ISLShmQueue* queue, int64 localId, int64 remoteId);
public:
	ITcpServer*		_server;
};

}
}

#endif