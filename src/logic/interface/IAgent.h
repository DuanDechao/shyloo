#ifndef __SL_INTERFACE_AGENT_H__
#define __SL_INTERFACE_AGENT_H__
#include "slimodule.h"

class IAgentListener{
public:
	virtual ~IAgentListener(){}

	virtual void onAgentOpen(sl::api::IKernel* pKernel, const int64 id) = 0;
	virtual void onAgentClose(sl::api::IKernel* pKernel, const int64 id) = 0;
	virtual int32 onAgentRecv(sl::api::IKernel* pKernel, const int64 id, const void* context, const int32 size) = 0;
};

class IAgent : public sl::api::IModule{
public:
	virtual ~IAgent() {}

	virtual void setListener(IAgentListener* pListener) = 0;

	virtual void send(const int64 id, const void* pBuf, const int32 size) = 0;
	virtual void kick(const int64 id) = 0;
};
#endif