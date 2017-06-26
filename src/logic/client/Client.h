#ifndef __SL_CORE_CLINET_H__
#define __SL_CORE_CLINET_H__
#include "IClient.h"
#include <unordered_map>
#include "slsingleton.h"
#include "slpool.h"
#include "ClientSession.h"

class Client : public IClient, public sl::SLHolder<Client>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void setListener(IClientListener* pListener);
	virtual void connect(const char* ip, const int32 port);
	virtual void close();
	virtual int32 recvSize(){ return _clientRecvSize; }
	virtual int32 sendSize(){ return _clientSendSize; }

	int32 onRecv(const char* pContext, const int32 size);
	void onOpen(ClientSession* pSession);
	void onClose();

	virtual void send(const void* pBuf, const int32 size);

private:
	sl::api::IKernel*		_kernel;
	IClientListener*		_listener;
	ClientSession*			_session;
	int32					_clientRecvSize;
	int32					_clientSendSize;
};
#endif