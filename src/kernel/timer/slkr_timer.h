#ifndef SL_KERNEL_TIMER_H
#define SL_KERNEL_TIMER_H
#include "slikernel.h"
#include "sltimer.h"
#include "slkernel.h"
#include "slobjectpool.h"
namespace sl
{
namespace core
{
class CKrTimer: public timer::ISLTimer, public api::ITimerBase{
public:
	virtual void SLAPI onInit(int64 timetick);
	virtual void SLAPI onStart(int64 timetick);
	virtual void SLAPI onTime(int64 timetick);
	virtual void SLAPI onTerminate(bool beForced, int64 timetick);
	virtual void SLAPI onPause(int64 timetick);
	virtual void SLAPI onResume(int64 timetick);

public:
	CKrTimer(){}
	virtual ~CKrTimer();

	void setITimer(api::ITimer* pITimer);

	void setTimerHandler(timer::SLTimerHandler timerHander) {m_timerHander = timerHander;}
	timer::SLTimerHandler getTimerHandler() const {return m_timerHander;}

private:
	api::ITimer*				m_pITimer;
	timer::SLTimerHandler		m_timerHander;
};
CREATE_OBJECT_POOL(CKrTimer);

}

}

#endif