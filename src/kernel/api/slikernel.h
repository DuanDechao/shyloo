#ifndef SL_KERNEL_IKERNEL_H
#define SL_KERNEL_IKERNEL_H
#include "sltimer.h"
namespace sl
{
namespace api
{
class IKernel;
class IPipe
{
public:
	virtual ~IPipe() {}
	virtual void send(const void* pContext, int dwLen) = 0;
	virtual void close() = 0;
};

class ISocket
{
public:
	virtual ~ISocket(){}
	IPipe* m_pPipe;
};

class ITcpSession: public ISocket
{
public:
	virtual ~ITcpSession(){}
	virtual void onRecv(IKernel* pKernel, const char* pContext, int dwLen) = 0;
};

class ITcpServer
{
public:
	virtual ~ITcpServer(){}
	virtual ITcpSession* mallocTcpSession(IKernel* pKernel) = 0;
};

class ITimerBase
{
public:
	virtual ~ITimerBase(){}
	virtual void setITimer(ITimer* pITimer) = 0;
};

class ITimer
{
public:
	virtual ~ITimer(){}

	virtual void onInit(IKernel* pKernel, int64 timetick){}
	virtual void onStart(IKernel* pKernel, int64 timetick) = 0;
	virtual void onTime(IKernel* pKernel, int64 timetick) = 0;
	virtual void onTerminate(IKernel* pKernel, int64 timetick) = 0;
	virtual void onPause(IKernel* pKernel, int64 timetick) = 0;
	virtual void onResume(IKernel* pKernel, int64 timetick) = 0;

	virtual void setITimerBase(ITimerBase* pITimerBase)
	{
		m_pITimerBase = pITimerBase;
	}
private:
	ITimerBase* m_pITimerBase;

};

class IKernel
{

};
}
}

#endif