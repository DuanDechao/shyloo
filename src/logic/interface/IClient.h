#ifndef __SL_INTERFACE_CLIENT_H__
#define __SL_INTERFACE_CLIENT_H__
#include "slimodule.h"

class IClientListener{
public:
	virtual ~IClientListener(){}

	virtual void onAgentOpen(sl::api::IKernel* pKernel, const int64 id) = 0;
	virtual void onAgentClose(sl::api::IKernel* pKernel, const int64 id) = 0;
	virtual int32 onAgentRecv(sl::api::IKernel* pKernel, const int64 id, const void* context, const int32 size) = 0;
};

class IClient : public sl::api::IModule{
public:
	virtual ~IClient() {}

	virtual void setListener(IClientListener* pListener) = 0;
	virtual void send(const int64 id, const void* pBuf, const int32 size) = 0;
	virtual void connect(const char* ip, const int32 port) = 0;
	virtual int32 recvSize() = 0;
	virtual int32 sendSize() = 0;
};
#endif