#ifndef SL_KERNEL_NET_SESSION_H
#define SL_KERNEL_NET_SESSION_H
#include "slnet.h"
#include "slikernel.h"
#include "slobjectpool.h"
#include "slbase_define.h"
namespace sl
{
namespace core
{
using namespace api;
using namespace network;
class NetSession: public ISLSession, public api::IPipe, public PoolObject
{
public:
	NetSession(){}
	NetSession(ITcpSession* pTcpSession);
	virtual ~NetSession();

	typedef SLShared_ptr<SmartPoolObject<NetSession>> SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj();
	static CObjectPool<NetSession>& ObjPool();
	static NetSession* createPoolObject();
	static void reclaimPoolObject(NetSession* obj);
	static void destroyObjPool();
	void onReclaimObject();
	virtual size_t getPoolObjectBytes();

	virtual void SLAPI setChannel(ISLChannel* pChannel);
	virtual void SLAPI release();
	virtual void SLAPI onRecv(const char* pBuf, uint32 dwLen);

	virtual void send(const void* pContext, int dwLen);
	virtual void close();

	virtual void setTcpSession(ITcpSession* pTcpSession) {m_pTcpSession = pTcpSession;}
private:
	ISLChannel*		m_pChannel;
	ITcpSession*	m_pTcpSession;
};

class ServerSessionFactory: public ISLSessionFactory
{
public:
	ServerSessionFactory(ITcpServer* pServer):m_pServer(pServer){}
	virtual ~ServerSessionFactory(){}
	virtual ISLSession* SLAPI createSession(ISLChannel* poChannel);
public:
	ITcpServer*		m_pServer;
};
}
}

#endif