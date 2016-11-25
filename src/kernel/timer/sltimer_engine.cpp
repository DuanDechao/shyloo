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

bool TimerEngine::startTimer(api::ITimer* pTimer, int64 delay, int32 count, int64 interval)
{
	CKrTimer* pKrTimer = CKrTimer::createPoolObject();
	pKrTimer->setITimer(pTimer);
	m_pTimerMgr->startTimer(pKrTimer, delay, count, interval);
}

}
}