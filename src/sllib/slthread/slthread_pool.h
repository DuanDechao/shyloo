#ifndef __SL_LIB_THREAD_POOL_H__
#define __SL_LIB_THREAD_POOL_H__
#include "slmulti_sys.h"
#include "slthread.h"
#include <queue>
#include <list>
namespace sl{
namespace thread
{
#define THREAD_BUSY_SIZE 32

class TPThread;
class SLThreadPool : public ISLThreadPool{
public:
	SLThreadPool(int32 newThreadCount, int32 defaultThreadCount, int32 maxThreadCount);
	~SLThreadPool();

	//获取当前线程总数
	virtual int32 SLAPI currentThreadCount(void) const { return m_currThreadCount; }

	//获取当前空闲线程总数
	virtual int32 SLAPI currentFreeThreadCount(void) const { return m_currFreeThreadCount; }

	//线程池是否处于繁忙状态
	virtual bool SLAPI isBusy(void) const { return m_bufferedTaskList.size() > THREAD_BUSY_SIZE; }

	//获得缓存的任务数量
	virtual int32 SLAPI bufferTaskSize() const { return (int32)m_bufferedTaskList.size(); }

	//获得已经已经完成任务数量
	virtual int32 SLAPI finiTaskSize() const { return (int32)m_finiTaskListCount; }

	virtual bool SLAPI isDestroyed(void) const { return m_isDestroyed; }

	virtual void SLAPI release();

	//启动
	virtual bool SLAPI start();

	//向线程池添加一个任务
	virtual bool SLAPI addTask(ITPTask* pTask);

	//run
	virtual bool SLAPI run(int64 overTime);

	virtual bool mainThreadTick();

	bool isThreadCountMax(void) const { return m_currThreadCount >= m_maxThreadCount; }

	//创建一个线程池线程
	TPThread* createThread(int32 threadWaitSecond);
	
	//将某个任务保存到未处理列表
	void bufferTask(ITPTask* pTask);

	//将未处理列表取出一个任务
	ITPTask* popBufferTask(void);

	//移动一个线程到空闲列表
	bool addFreeThread(TPThread* td);

	//移动一个线程到繁忙列表
	bool addBusyThread(TPThread* td);

	//添加一个已经完成的任务到列表
	void addFiniTask(ITPTask* pTask);

	//删除一个挂起（超时）的线程
	bool removeHangThread(TPThread* td);

	bool hasThread(TPThread* td);

private:
	void destroy();

private:
	bool					m_isInitialize;				//线程池是否已经初始化
	bool					m_isDestroyed;				//是否被销毁

	std::queue<ITPTask*>	m_bufferedTaskList;			//等待处理的任务列表
	std::list<ITPTask*>		m_finiTaskList;				//已经处理完的任务列表
	int32					m_finiTaskListCount;

	CRITICAL_SECTION		m_bufferedTaskListMutex;	//等待处理任务列表的互斥锁
	CRITICAL_SECTION		m_threadStateListMutex;		//切换线程状态的互斥锁
	CRITICAL_SECTION		m_finiTaskListMutex;		//处理完的任务列表的互斥锁

	std::list<TPThread*>    m_busyThreadList;			//繁忙的线程列表
	std::list<TPThread*>	m_freeThreadList;			//空闲的线程列表
	std::list<TPThread*>	m_allThreadList;			//所有的线程列表

	int32					m_maxThreadCount;			//最大线程数
	int32					m_extraNewAddThreadCount;	//线程饱和时，一次增加的线程数
	int32					m_currThreadCount;			//当前线程数
	int32					m_currFreeThreadCount;		//当前空闲的线程数
	int32					m_defaultThreadCount;		//初始默认开启的线程数
};
}
}

#endif