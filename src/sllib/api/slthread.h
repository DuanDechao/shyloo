#ifndef __SL_LIB_SLTHREAD_H__
#define __SL_LIB_SLTHREAD_H__
#include "sltype.h"

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
	virtual void SLAPI release() = 0;
};

class ISLThreadPool{
public:
	//获取当前线程总数
	virtual int32 SLAPI currentThreadCount(void) const = 0;

	//获取当前空闲线程总数
	virtual int32 SLAPI currentFreeThreadCount(void) const = 0;
	
	//线程池是否处于繁忙状态
	virtual bool SLAPI isBusy(void) const = 0;
	
	//获得缓存的任务数量
	virtual int32 SLAPI bufferTaskSize() const = 0;
	
	//获得已经已经完成任务数量
	virtual int32 SLAPI finiTaskSize() const = 0;

	virtual bool SLAPI isDestroyed(void) const = 0;

	//启动
	virtual bool SLAPI start() = 0;

	//向线程池添加一个任务
	virtual bool SLAPI addTask(ITPTask* pTask) = 0;

	//主线程执行
	virtual bool SLAPI run(int64 overTime) = 0;

	//释放
	virtual void SLAPI release() = 0;
};

ISLThreadPool* SLAPI createThreadPool(int32 newThreadCount, int32 defaultThreadCount, int32 maxThreadCount);

}
}
#endif