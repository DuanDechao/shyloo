#ifndef SL_KERNEL_IKERNEL_H
#define SL_KERNEL_IKERNEL_H
#include "slmulti_sys.h"
#include "slthread.h"
#include "sldb.h"
#include "slargs.h"
#include <functional>
namespace sl
{
namespace api
{
class IKernel;
class IModule;
class IPipe{
public:
	virtual ~IPipe() {}
	virtual void send(const void* pContext, int dwLen) = 0;
	virtual void close() = 0;

	virtual const char* getRemoteIP() = 0;
};

class ISocket{
public:
	virtual ~ISocket(){}
	IPipe* m_pPipe;
};

class ITcpSession: public ISocket{
public:
	virtual ~ITcpSession(){}
	virtual int32 onRecv(IKernel* pKernel, const char* pContext, int dwLen) = 0;
	virtual void onConnected(IKernel* pKernel) = 0;
	virtual void onDisconnect(IKernel* pKernel) = 0;

	inline void send(const void* pContext, const int32 size){
		if (m_pPipe)
			m_pPipe->send(pContext, size);
	}

	inline const char* getRemoteIP(){
		if (m_pPipe)
			return m_pPipe->getRemoteIP();
		return "";
	}

	inline void close(){
		if (m_pPipe)
			m_pPipe->close();
	}
};

class ITcpServer{
public:
	virtual ~ITcpServer(){}
	virtual ITcpSession* mallocTcpSession(IKernel* pKernel) = 0;
};

class ITimer;
class ITimerBase{
public:
	virtual ~ITimerBase(){}
	virtual void setITimer(ITimer* pITimer) = 0;
};

class ITimer{
public:
	virtual ~ITimer(){}

	virtual void onInit(IKernel* pKernel, int64 timetick){}
	virtual void onStart(IKernel* pKernel, int64 timetick) = 0;
	virtual void onTime(IKernel* pKernel, int64 timetick) = 0;
	virtual void onTerminate(IKernel* pKernel, bool beForced, int64 timetick) = 0;
	virtual void onPause(IKernel* pKernel, int64 timetick) = 0;
	virtual void onResume(IKernel* pKernel, int64 timetick) = 0;

	virtual ITimerBase* getITimerBase() const {return m_pITimerBase;}

	virtual void setITimerBase(ITimerBase* pITimerBase){
		m_pITimerBase = pITimerBase;
	}
private:
	ITimerBase* m_pITimerBase;

};

class IAsyncBase {
public:
	virtual ~IAsyncBase() {}
};

class IAsyncHandler{
public:
	IAsyncHandler() :_base(nullptr){}
	virtual ~IAsyncHandler() {}

	inline void setBase(IAsyncBase* base) { _base = base; }
	inline IAsyncBase* getBase() { return _base; }

	virtual bool onExecute(IKernel* pKernel) = 0;
	virtual bool onSuccess(IKernel* pKernel) = 0;
	virtual bool onFailed(IKernel* pKernel, bool nonviolent) = 0;
	virtual void onRelease(IKernel* pKernel) = 0;

private:
	IAsyncBase* _base;
};

class IKernel{
public:
	virtual bool ready() = 0;
	virtual bool initialize(int32 argc, char ** argv) = 0;
	virtual bool destory() = 0;

	//net interface
	virtual bool startTcpServer(api::ITcpServer * server, const char* ip, const int32 port, int32 sendSize, int32 recvSize) = 0;
	virtual bool startTcpClient(api::ITcpSession * client, const char* ip, const int32 port, int32 sendSize, int32 recvSize) = 0;
	virtual const char* getInternetIp() = 0;

	//timer interface
	virtual bool startTimer(api::ITimer* timer, int64 delay, int32 count, int64 interval) = 0;
	virtual bool killTimer(api::ITimer* timer) = 0;
	virtual void pauseTimer(api::ITimer* timer) = 0;
	virtual void resumeTimer(api::ITimer* timer) = 0;

	// logic interface
	virtual IModule * findModule(const char * name) = 0;
	virtual const char* getCmdArg(const char* name) = 0;
	
	//config interface
	virtual const char* getCoreFile() = 0;
	virtual const char* getConfigFile() = 0;
	virtual const char* getEnvirPath() = 0;

	//async interface
	virtual void startAsync(const int64 threadId, IAsyncHandler* handler, const char* debug) = 0;
	virtual void stopAsync(IAsyncHandler* handler) = 0;

	//log interface
	virtual void syncLog(int32 filter, const char* log, const char* file, const int32 line) = 0;
	virtual void asyncLog(int32 filter, const char* log, const char* file, const int32 line) = 0;

};
}
}

#define START_TIMER(timer, delay, count, interval){		\
	pKernel->startTimer(timer, delay, count, interval);	\
}

#define FIND_MODULE(m, name) {\
	m = (I##name *)pKernel->findModule(#name); \
	SLASSERT(m, "where is %s", #name); \
}
#define TIMER_BEAT_FOREVER	-1
#endif