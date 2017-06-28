#ifndef SL_KERNEL_NET_SESSION_H
#define SL_KERNEL_NET_SESSION_H
#include "slnet.h"
#include "slikernel.h"
#include "slobjectpool.h"
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
	NetSession(){}
	NetSession(ITcpSession* pTcpSession);
	virtual ~NetSession();

	virtual void SLAPI setChannel(ISLChannel* pChannel);
	virtual void SLAPI release();
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
	ISLChannel*		_channel;
	ITcpSession*	_tcpSession;
};

class ServerSessionFactory: public ISLSessionFactory{
public:
	ServerSessionFactory(ITcpServer* pServer) : _server(pServer){}
	virtual ~ServerSessionFactory(){}
	virtual ISLSession* SLAPI createSession(ISLChannel* poChannel);

private:
	ITcpServer*		_server;
};

CREATE_OBJECT_POOL(NetSession);

}
}

#endif