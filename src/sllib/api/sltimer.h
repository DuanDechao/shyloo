#ifndef SL_SLTIMER_H
#define SL_SLTIMER_H
#include "sltype.h"
namespace sl
{
namespace timer
{

class ISLTimer{
public:
	virtual void SLAPI onInit(int64 timetick) = 0;
	virtual void SLAPI onStart(int64 timetick) = 0;
	virtual void SLAPI onTime(int64 timetick) = 0;
	virtual void SLAPI onTerminate(bool beForced, int64 timetick) = 0;
	virtual void SLAPI onPause(int64 timetick) = 0;
	virtual void SLAPI onResume(int64 timetick) = 0;
};

typedef void* SLTimerHandler;
#define INVALID_TIMER_HANDER nullptr

class ISLTimerMgr{
public:
	virtual SLTimerHandler SLAPI startTimer(ISLTimer* pTimer, int64 delay, int32 count, int64 interval, const char* debug) = 0;
	virtual bool SLAPI killTimer(SLTimerHandler pTimer) = 0;
	virtual void SLAPI pauseTimer(SLTimerHandler pTimer) = 0;
	virtual void SLAPI resumeTimer(SLTimerHandler pTimer) = 0;
	virtual int64 SLAPI process(int64 overTime) = 0;
	virtual void SLAPI release() = 0;
};

extern "C" SL_DLL_API ISLTimerMgr* SLAPI getSLTimerModule(void);
}
}
#endif
