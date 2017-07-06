#include "sltimer_base.h"
#include "sltimer_mgr.h"
#include "sltime.h"

namespace sl
{
namespace timer
{
sl::SLPool<CSLTimerBase> CSLTimerBase::s_pool;
CSLTimerBase::CSLTimerBase(ISLTimer* pTimer, jiffies_t delay, int32 count, jiffies_t interval)
	:m_stat(TIME_RECREATE),
	m_pause(0),
	m_pTimerObj(pTimer),
	m_expire(delay),
	m_bDelay(true),
	m_iCount(count),
	m_interval(interval),
	m_recursion(0)
{}

CSLTimerBase::~CSLTimerBase(){
	m_stat = TIME_DESTORY;
	m_pause = 0;
	m_pTimerObj = nullptr;
	m_expire = 0;
	m_bDelay = true;
	m_iCount = 0;
	m_interval = 0;
}

void CSLTimerBase::adjustExpireTime(jiffies_t nowJiffies){
	int64 live = m_expire - nowJiffies;
	if (live < 0 && m_interval > 0 && (uint64)abs(live) > m_interval){
		m_expire += (uint64)abs(live) / m_interval;
	}
}

CSLTimerBase::TimerState CSLTimerBase::pollTimer(){
	if (!good())
		return TimerState::TIME_DESTORY;

	if(m_bDelay){
		onStart();

		if (m_stat == TimerState::TIME_DESTORY)
			return  TimerState::TIME_DESTORY;

		return TimerState::TIME_RECREATE;
	}
	else{
		if (m_iCount != 0)
			onTimer();

		if (m_stat == TimerState::TIME_DESTORY)
			return  TimerState::TIME_DESTORY;

		if(m_iCount > 0)
			--m_iCount;

		if(m_iCount == 0)
			return TimerState::TIME_REMOVE;

		return TimerState::TIME_RECREATE;
	}
}

void CSLTimerBase::onInit(){
	m_recursion++;
	m_pTimerObj->onInit((int64)sl::getTimeMilliSecond());
	m_recursion--;
}

void CSLTimerBase::onStart(){
	m_recursion++;
	m_pTimerObj->onStart((int64)sl::getTimeMilliSecond());
	m_recursion--;
	m_bDelay = false;
}

void CSLTimerBase::onTimer(){
	m_recursion++;
	m_pTimerObj->onTime((int64)sl::getTimeMilliSecond());
	m_recursion--;
}

void CSLTimerBase::onPause(){
	m_recursion++;
	m_pTimerObj->onPause((int64)sl::getTimeMilliSecond());
	m_recursion--;
}

void CSLTimerBase::onResume(){
	m_recursion++;
	m_pTimerObj->onResume((int64)sl::getTimeMilliSecond());
	m_recursion--;
}

void CSLTimerBase::onEnd(bool beForced){
	setTimerState(TimerState::TIME_DESTORY);
	m_pTimerObj->onTerminate(beForced, (int64)sl::getTimeMilliSecond());

	if (m_recursion <= 0)
		release();
}

}
}
