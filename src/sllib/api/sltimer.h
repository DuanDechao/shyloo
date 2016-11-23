#ifndef SL_SLTIMER_H
#define SL_SLTIMER_H
#include "sltype.h"
namespace sl
{
namespace timer
{
class ISLTimer
{
public:
	virtual void SLAPI onInit(int64 timetick) = 0;
	virtual void SLAPI onStart(int64 timetick) = 0;
	virtual void SLAPI onTime(int64 timetick) = 0;
	virtual void SLAPI onTerminate(int64 timetick) = 0;
	virtual void SLAPI onPause(int64 timetick) = 0;
	virtual void SLAPI onResume(int64 timetick) = 0;
};

class ISLTimerMgr
{
public:
	virtual bool SLAPI startTimer(ISLTimer* pTimer, int64 delay, int32 count, int64 interval) = 0;
	virtual bool SLAPI killTimer(ISLTimer* pTimer) = 0;
	virtual void SLAPI pauseTimer(ISLTimer* pTimer) = 0;
	virtual void SLAPI resumeTimer(ISLTimer* pTimer) = 0;
};
}
}
#endif
