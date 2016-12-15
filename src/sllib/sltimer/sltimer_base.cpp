#include "sltimer_base.h"
#include "sltimer_mgr.h"
namespace sl
{
namespace timer
{
CSLTimerBase::CSLTimerBase(TimersBase* owner, ISLTimer* pTimer, int64 delay, int32 count, int64 interval)
	:m_Owner(owner),
	 m_stat(TIME_RECREATE),
	 m_pauseStamp(0),
	 m_pTimerObj(pTimer),
	 m_expireStamp(0),
	 m_bDelay(true),
	 m_iCount(count),
	 m_intervalStamp(TimeStamp::fromMilliSeconds(interval))
{
	setExpireTime(timestamp() + TimeStamp::fromMilliSeconds(delay));
}

CSLTimerBase::~CSLTimerBase()
{
	m_stat = TIME_DESTORY;
	m_pauseStamp = 0;
	m_pTimerObj = nullptr;
	m_expireStamp = 0;
	m_bDelay = true;
	m_iCount = 0;
	m_intervalStamp = 0;
}

CSLTimerBase::TimerState CSLTimerBase::pollTimer()
{
	if(!good())
		return TimerState::TIME_DESTORY;

	if(m_bDelay)
	{
		onStart();

		return TimerState::TIME_RECREATE;
	}
	else{
		if(m_iCount != 0)
			onTimer();

		if(m_iCount > 0)
			--m_iCount;

		if(m_iCount == 0)
			return TimerState::TIME_DESTORY;

		return TimerState::TIME_RECREATE;
	}
}

void CSLTimerBase::onInit()
{
	uint64 nowTime = timestamp() / stampsPerSecond();
	m_pTimerObj->onInit((int64)nowTime);
}

void CSLTimerBase::onStart()
{
	uint64 nowTime = timestamp() / stampsPerSecond();
	m_pTimerObj->onStart((int64)nowTime);
	m_bDelay = false;
}

void CSLTimerBase::onTimer()
{
	uint64 nowTime = timestamp() / stampsPerSecond();
	m_pTimerObj->onTime(nowTime);
}

void CSLTimerBase::onPause()
{
	uint64 nowTime = timestamp() / stampsPerSecond();
	m_pTimerObj->onPause(nowTime);
}

void CSLTimerBase::onResume()
{
	uint64 nowTime = timestamp() / stampsPerSecond();
	m_pTimerObj->onResume(nowTime);
}

void CSLTimerBase::onEnd()
{
	uint64 nowTime = timestamp() / stampsPerSecond();
	m_pTimerObj->onTerminate(nowTime);
}

void CSLTimerBase::release() 
{
	m_Owner->onCancel();
	m_stat = TIME_DESTORY;
	onEnd();
}


}
}