#include "sltimer_engine.h"
namespace sl
{
SL_SINGLETON_INIT(core::TimerEngine);
namespace core
{
bool TimerEngine::initialize()
{
	m_pTimerMgr = timer::getSLTimerModule();
	if(nullptr == m_pTimerMgr)
		return false;
	return true;	
}

timer::SLTimerHandler TimerEngine::getTimerHander(api::ITimer* pTimer)
{
	if(nullptr == pTimer)
		return nullptr;

	CKrTimer* pTimerBase = (CKrTimer*)pTimer->getITimerBase();
	if(nullptr == pTimerBase)
		return nullptr;

	return pTimerBase->getTimerHandler();
}

bool TimerEngine::startTimer(api::ITimer* pTimer, int64 delay, int32 count, int64 interval)
{
	CKrTimer* pKrTimer = CKrTimer::createPoolObject();
	if(nullptr == pKrTimer)
		return false;

	pTimer->setITimerBase(pKrTimer);
	pKrTimer->setITimer(pTimer);

	timer::SLTimerHandler timerHandler = m_pTimerMgr->startTimer(pKrTimer, delay, count, interval);
	if(nullptr == timerHandler)
		return false;

	pKrTimer->setTimerHandler(timerHandler);

	return true;
}

bool TimerEngine::killTimer(api::ITimer* pTimer)
{
	timer::SLTimerHandler timerHandler  = getTimerHander(pTimer);
	if(INVALID_TIMER_HANDER == timerHandler)
		return false;

	return m_pTimerMgr->killTimer(timerHandler);
}

bool TimerEngine::pauseTimer(api::ITimer* pTimer)
{
	timer::SLTimerHandler timerHandler  = getTimerHander(pTimer);
	if(INVALID_TIMER_HANDER == timerHandler)
		return false;

	m_pTimerMgr->pauseTimer(timerHandler);
	return true;
}

bool TimerEngine::resumeTimer(api::ITimer* pTimer)
{
	timer::SLTimerHandler timerHandler  = getTimerHander(pTimer);
	if(INVALID_TIMER_HANDER == timerHandler)
		return false;

	m_pTimerMgr->resumeTimer(timerHandler);
	return true;
}

int64 TimerEngine::processing()
{
	int64 beginTime = TimeStamp::toSeconds(timestamp());
	m_pTimerMgr->process(timestamp());
	int64 endTime = TimeStamp::toSeconds(timestamp());
	return endTime - beginTime;
}

}
}