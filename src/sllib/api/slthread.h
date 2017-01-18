#ifndef __SL_LIB_SLTHREAD_H__
#define __SL_LIB_SLTHREAD_H__
#include "slmulti_sys.h"

namespace sl
{
namespace thread
{
enum TPTaskState{
	//任务完成
	TPTASK_STATE_COMPLETED,

	//任务需继续在主线程执行
	TPTASK_STATE_CONTINUE_MAINTHREAD,

	//任务需继续在子线程执行
	TPTASK_STATE_CONTINUE_CHILDTHREAD,
};

class ITPTask{
public:
	virtual bool SLAPI start() = 0;
	virtual bool SLAPI process() = 0;
	virtual bool SLAPI end() = 0;
	virtual TPTaskState SLAPI presentMainThread() = 0;
};

class ISLThreadPool{
public:
	//获取当前线程总数
	virtual int32 SLAPI currentThreadCount(void) const;

	//获取当前空闲线程总数
	virtual int32 SLAPI currentFreeThreadCount(void) const;
	
	//线程池是否处于繁忙状态
	virtual bool SLAPI isBusy(void) const;
	
	//获得缓存的任务数量
	virtual int32 SLAPI bufferTaskSize() const;
	
	//获得已经已经完成任务数量
	virtual int32 SLAPI finiTaskSize() const;

	virtual bool SLAPI isDestroyed(void) const;

	//启动
	virtual bool SLAPI start();

	//向线程池添加一个任务
	virtual bool SLAPI addTask(ITPTask* pTask);

	//主线程执行
	virtual void SLAPI onMainThreadTick();

	//释放
	virtual void SLAPI release();
};

ISLThreadPool* SLAPI createThreadPool(int32 newThreadCount, int32 defaultThreadCount, int32 maxThreadCount);

}
}
#endif