#ifndef __SL_CORE_CLINET_H__
#define __SL_CORE_CLINET_H__
#include "IClient.h"
#include <unordered_map>
#include "slsingleton.h"

class Client;
class ClientSessionPool : public sl::api::ITcpServer{
public:
	ClientSessionPool(Client* pClient) :m_pClient(pClient){}
	virtual ~ClientSessionPool(){}
	virtual sl::api::ITcpSession* mallocTcpSession(sl::api::IKernel* pKernel);

private:
	Client* m_pClient;
};

class ClientSession;
class Client : public IClient, public sl::SLHolder<Client>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void setListener(IClientListener* pListener);
	virtual void connect(const char* ip, const int32 port);
	virtual int32 recvSize(){ return _clientRecvSize; }
	virtual int32 sendSize(){ return _clientSendSize; }

	int32 onRecv(int64 id, const char* pContext, const int32 size);
	int64 onOpen(ClientSession* pSession);
	void onClose(int64 id);

	virtual void send(const int64 id, const void* pBuf, const int32 size);

private:
	sl::api::IKernel*   _kernel;
	int64				_clientNextId;
	IClientListener*	_listener;
	int32				_clientRecvSize;
	int32				_clientSendSize;
	std::unordered_map<int64, ClientSession*> _clientSessions;
	ClientSessionPool*	_sessionPool;
};
#endif