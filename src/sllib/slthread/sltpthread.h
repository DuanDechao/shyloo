#ifndef __SL_LIB_TPTHREAD_H__
#define __SL_LIB_TPTHREAD_H__
#include "slmulti_sys.h"
#include "slthread.h"
namespace sl
{
namespace thread
{
class SLThreadPool;
class TPThread{
public:
	enum THREAD_STATE{
		THREAD_STATE_STOP = -1,
		THREAD_STATE_SLEEP = 0,
		THREAD_STATE_BUSY = 1,
		THREAD_STATE_END = 2
	};

public:
	TPThread(SLThreadPool* threadPool, int32 threadWaitSecond = 0)
		:m_threadWaitSecond(threadWaitSecond),
		m_currTask(nullptr),
		m_threadPool(threadPool)
	{
		m_state = THREAD_STATE_SLEEP;
		initCond();
		intMutex();
	}

	~TPThread(){
		deleteCond();
		deleteMutex();
	}

	inline HANDLE id() const { return m_threadId; }
	inline void setId(HANDLE tid) { m_threadId = tid; }
	//获得本线程要处理的任务
	inline ITPTask* task(void) const{ return m_currTask; }
	//设置本线程要处理的任务
	inline void setTask(ITPTask* pTask){ m_currTask = pTask; }
	//当前状态
	inline int32 state(void) const { return m_state; }
	inline void setState(THREAD_STATE state){ m_state = state; }
	inline SLThreadPool* threadPool(void) const { return m_threadPool; }

	void onTPTaskStart(){ SLASSERT(m_currTask, "wtf"); m_currTask->start(); }
	void onTPTaskProcess(){ SLASSERT(m_currTask, "wtf"); m_currTask->process(); }
	void onTPTaskEnd(){ SLASSERT(m_currTask, "wtf"); m_currTask->end(); }


	HANDLE createThread(void);

	//当前任务处理完毕
	void onTaskCompleted(void);

	//线程通知 等待条件信号
	bool onWaitCondSignal(void);

	bool join(void);

	ITPTask* tryGetTask(void);

	//更新持续执行的任务数
	inline void resetDoneTasks() { m_doneTasks = 0; }
	inline void incDoneTasks(){ ++m_doneTasks; }

#ifdef SL_OS_WINDOWS
	static unsigned __stdcall threadFunc(void *args);
#else
	static void* threadFunc(void* args);
#endif

	inline void initCond(void){
		m_cond = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	inline void intMutex(void){
		InitializeCriticalSection(&m_mutex);
	}

	inline void deleteCond(void){
		CloseHandle(m_cond);
	}

	inline void deleteMutex(void){
		DeleteCriticalSection(&m_mutex);
	}

	inline void lock(void){
		EnterCriticalSection(&m_mutex);
	}

	inline void unLock(void){
		LeaveCriticalSection(&m_mutex);
	}

	inline int32 sendCondSignal(void){
		return SetEvent(m_cond);
	}

private:
	HANDLE					m_cond;				//线程信号量
	CRITICAL_SECTION		m_mutex;			//线程互斥锁
	int32					m_threadWaitSecond;	//线程空闲最长等待时间，超过就删除
	ITPTask*				m_currTask;			//线程当前执行的任务
	HANDLE					m_threadId;			//线程ID
	SLThreadPool*			m_threadPool;		//所属线程池
	THREAD_STATE			m_state;			//线程状态 -1表示还未启动
	int32					m_doneTasks;		//线程持续执行任务数量
};
}
}

#endif