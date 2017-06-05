#include "sltimer_engine.h"
namespace sl
{
namespace core
{

bool TimerEngine::initialize(){
	m_pTimerMgr = timer::getSLTimerModule();
	if (nullptr == m_pTimerMgr){
		SLASSERT(false, "wtf");
		return false;
	}
	return true;	
}

bool TimerEngine::ready()
{
	return true;
}

bool TimerEngine::destory()
{
	DEL this;
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
	CKrTimer* pKrTimer = CREATE_POOL_OBJECT(CKrTimer);
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

int64 TimerEngine::loop(int64 overTime){
	return m_pTimerMgr->process(overTime);
}

}
}