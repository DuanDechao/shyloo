#ifndef SL_KERNEL_IKERNEL_H
#define SL_KERNEL_IKERNEL_H
#include "slmulti_sys.h"
//#include "sltimer.h"
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

	virtual const char* getRemoteIP() = 0;
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
	virtual int32 onRecv(IKernel* pKernel, const char* pContext, int dwLen) = 0;
	virtual void onConnected() = 0;
	virtual void onTerminate() = 0;

	inline void send(const void* pContext, const int32 size){
		if (m_pPipe)
			m_pPipe->send(pContext, size);
	}

	inline const char* getRemoteIP(){
		if (m_pPipe)
			return m_pPipe->getRemoteIP();
		return "";
	}
};

class ITcpServer
{
public:
	virtual ~ITcpServer(){}
	virtual ITcpSession* mallocTcpSession(IKernel* pKernel) = 0;
};

class ITimer;
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

	virtual ITimerBase* getITimerBase() const {return m_pITimerBase;}

	virtual void setITimerBase(ITimerBase* pITimerBase)
	{
		m_pITimerBase = pITimerBase;
	}
private:
	ITimerBase* m_pITimerBase;

};

class IKernel
{
public:
	//net interface
	virtual bool startTcpServer(api::ITcpServer * server, const char* ip, const int32 port, int32 sendSize, int32 recvSize) = 0;
	virtual bool startTcpClient(api::ITcpSession * client, const char* ip, const int32 port, int32 sendSize, int32 recvSize) = 0;

	//timer interface
	virtual bool startTimer(api::ITimer* timer, int64 delay, int32 count, int64 interval) = 0;
	virtual bool killTimer(api::ITimer* timer) = 0;
	virtual void pauseTimer(api::ITimer* timer) = 0;
	virtual void resumeTimer(api::ITimer* timer) = 0;
};
}
}

#define START_TIMER(timer, delay, count, interval){		\
	pKernel->startTimer(timer, delay, count, interval);	\
}

#define TIMER_BEAT_FOREVER	-1
#endif