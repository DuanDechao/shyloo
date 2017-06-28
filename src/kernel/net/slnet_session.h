#ifndef SL_KERNEL_NET_SESSION_H
#define SL_KERNEL_NET_SESSION_H
#include "slnet.h"
#include "slikernel.h"
#include "slpool.h"

namespace sl{
namespace core{

using namespace api;
using namespace network;
class NetPacketParser : public ISLPacketParser{
public:
	virtual int32 SLAPI parsePacket(const char* pDataBuf, int32 len);
};

class NetSession: public ISLSession, public api::IPipe{
public:
	inline static NetSession* create(ITcpSession* pTcpSession){
		return CREATE_FROM_POOL(s_pool, pTcpSession);
	}

	virtual void SLAPI setChannel(ISLChannel* pChannel){ _channel = pChannel; }
	virtual void SLAPI release(){ s_pool.recover(this); }
	virtual void SLAPI onRecv(const char* pBuf, uint32 dwLen);
	virtual void SLAPI onEstablish(void);
	virtual void SLAPI onTerminate();
	
	virtual const char* getRemoteIP();
	virtual void send(const void* pContext, int dwLen);
	virtual void close();
	virtual void adjustSendBuffSize(const int32 size);
	virtual void adjustRecvBuffSize(const int32 size);

	inline void setTcpSession(ITcpSession* pTcpSession) { _tcpSession = pTcpSession; }

private:
	friend sl::SLPool<NetSession>;
	NetSession(ITcpSession* pTcpSession);
	virtual ~NetSession();

private:
	ISLChannel*		_channel;
	ITcpSession*	_tcpSession;
	static sl::SLPool<NetSession> s_pool;
};

class ServerSessionFactory: public ISLSessionFactory{
public:
	ServerSessionFactory(ITcpServer* pServer) : _server(pServer){}
	virtual ~ServerSessionFactory(){}
	virtual ISLSession* SLAPI createSession(ISLChannel* poChannel);

private:
	ITcpServer*		_server;
};

}
}

#endif