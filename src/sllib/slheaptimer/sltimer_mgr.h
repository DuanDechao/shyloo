#ifndef SL_SLTIMER_MGR_H
#define SL_SLTIMER_MGR_H
#include "sltime.h"
#include "sltimer.h"
#include "slsingleton.h"
#include "sltimer_base.h"
#include "slpriority_queue.h"
namespace sl{
namespace timer{
class SLTimerMgr : public ISLTimerMgr, public CSingleton<SLTimerMgr>{
public:
	virtual SLTimerHandler SLAPI startTimer(ISLTimer* pTimer, int64 delay, int32 count, int64 interval, const char* debug);
	virtual bool SLAPI killTimer(SLTimerHandler pTimer);
	virtual void SLAPI pauseTimer(SLTimerHandler pTimer);
	virtual void SLAPI resumeTimer(SLTimerHandler pTimer);
	virtual int64 SLAPI process(int64 overTime);
	virtual void SLAPI release();
	virtual int64 SLAPI getNextExp(int64 maxTime);

public:
	SLTimerMgr():_numPurged(0){}
	virtual ~SLTimerMgr(){}

	void schedule(CSLTimerBase* timerBase);

private:
	void reCreateTimer(CSLTimerBase* pTimer);
	void endTimer(CSLTimerBase* pTimer);
	inline int64 getJiffies() {return sl::getTimeMilliSecond();}
	void purgeCancelledTimes();

private:
	SLTimerMgr(const SLTimerMgr&);
	SLTimerMgr& operator=(const SLTimerMgr&);

private:
	SLPriorityQueue _timerQueue;	
	int32			_numPurged;

};

}
}
#endif
