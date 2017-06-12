#ifndef SL_TIMER_ENGINE_H
#define SL_TIMER_ENGINE_H
#include "slitimer_engine.h"
#include "sltimer.h"
#include "slkr_timer.h"
#include "slsingleton.h"
namespace sl
{
namespace core
{
class TimerEngine: public SLSingleton<TimerEngine>{
	friend class SLSingleton<TimerEngine>;
public:
	virtual bool ready();
	virtual bool initialize();
	virtual bool destory();

	virtual bool startTimer(api::ITimer* pTimer, int64 delay, int32 count, int64 interval, const char* file, const int32 line);
	virtual bool killTimer(api::ITimer* pTimer);
	virtual bool pauseTimer(api::ITimer* pTimer);
	virtual bool resumeTimer(api::ITimer* pTimer);
	virtual int64 loop(int64 overTime);

	timer::SLTimerHandler getTimerHander(api::ITimer* pTimer);

private:
	TimerEngine(){}
	~TimerEngine(){}

private:
	timer::ISLTimerMgr*		m_pTimerMgr;

};
}
}

#endif