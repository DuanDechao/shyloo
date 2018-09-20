#define SL_DLL_EXPORT
#include "sltimer_mgr.h"
namespace sl
{
SL_SINGLETON_INIT(timer::SLTimerMgr);
namespace timer
{

class IsNotCancelled{
public:
	bool operator()(const CSLTimerBase* pTimer){
		return pTimer->getTimerState() != CSLTimerBase::TimerState::TIME_DESTORY; 
	}
};
extern "C" SL_DLL_API ISLTimerMgr* SLAPI getSLTimerModule(){
	SLTimerMgr* g_netTimerPtr = SLTimerMgr::getSingletonPtr();
	if (g_netTimerPtr == NULL)
		g_netTimerPtr = NEW SLTimerMgr();
	return SLTimerMgr::getSingletonPtr();
}

void SLTimerMgr::release(){
	DEL this;
}


SLTimerHandler SLTimerMgr::startTimer(ISLTimer* pTimer, int64 delay, int32 count, int64 interval, const char* debug){
	CSLTimerBase* pMgrTimerObj = CSLTimerBase::create(pTimer, (jiffies_t)(getJiffies() + delay), count, (jiffies_t)interval);
	SLASSERT(pMgrTimerObj && pMgrTimerObj->good(), "create timerbase failed");

	pMgrTimerObj->setDebug(debug);
	pMgrTimerObj->onInit();
	schedule(pMgrTimerObj);
	
	return pMgrTimerObj;
}

bool SLTimerMgr::killTimer(SLTimerHandler pTimer){
	if (INVALID_TIMER_HANDER == pTimer){
		SLASSERT(false, "wtf");
		return false;
	}

	CSLTimerBase* pTimerBase = (CSLTimerBase*)pTimer;
	if (!pTimerBase->good()){
		SLASSERT(false, "wtf");
		return false;
	}

	pTimerBase->onEnd(true);
	
	++_numPurged;
	if(_numPurged * 2 > _timerQueue.size())
		purgeCancelledTimes();

	return true;
}

void SLTimerMgr::pauseTimer(SLTimerHandler pTimer){
	if (INVALID_TIMER_HANDER == pTimer){
		SLASSERT(false, "wtf");
		return;
	}

	CSLTimerBase* pTimerBase = (CSLTimerBase*)pTimer;
	if (!pTimerBase->good()){
		SLASSERT(false, "wtf");
		return;
	}

	pTimerBase->setTimerState(CSLTimerBase::TimerState::TIME_PAUSED);
	pTimerBase->setPauseTime(getJiffies());
	pTimerBase->onPause();
}

void SLTimerMgr::resumeTimer(SLTimerHandler pTimer){
	if(INVALID_TIMER_HANDER == pTimer){
		SLASSERT(false, "wtf");
		return;
	}

	CSLTimerBase* pTimerBase = (CSLTimerBase*)pTimer;
	if (!pTimerBase->good() || pTimerBase->getTimerState() != CSLTimerBase::TimerState::TIME_PAUSED){
		SLASSERT(false, "wtf");
		return;
	}

	pTimerBase->setTimerState(CSLTimerBase::TimerState::TIME_RECREATE);
	pTimerBase->setExpireTime(getJiffies() + pTimerBase->getExpireTime() - pTimerBase->getPauseTime());
	_timerQueue.makeHeap();

	pTimerBase->onResume();
}

int64 SLTimerMgr::process(int64 overTime){
	int64 tick = sl::getTimeMilliSecond();
	while(!_timerQueue.empty() && _timerQueue.top()->getExpireTime() <= tick){
		if(sl::getTimeMilliSecond() - tick > overTime)
			break;

		CSLTimerBase* pTimer = _timerQueue.top();
		_timerQueue.pop();

		if(!pTimer)
			continue;
		
		CSLTimerBase::TimerState state = pTimer->pollTimer();
		switch (state){
		case sl::timer::CSLTimerBase::TIME_RECREATE: reCreateTimer(pTimer); break;
		case sl::timer::CSLTimerBase::TIME_REMOVE: endTimer(pTimer); break;
		case sl::timer::CSLTimerBase::TIME_DESTORY: pTimer->release(); break;
		case sl::timer::CSLTimerBase::TIME_PAUSED: break;
		default: break;
		}
	}

	return sl::getTimeMilliSecond() - tick;
}

void SLTimerMgr::reCreateTimer(CSLTimerBase* pTimer){
	pTimer->setExpireTime(pTimer->getExpireTime() + pTimer->getIntervalTime());
	pTimer->adjustExpireTime(getJiffies());
	schedule(pTimer);
}

void SLTimerMgr::endTimer(CSLTimerBase* pTimer){
	SLASSERT(pTimer->good(), "wtf");
	pTimer->onEnd(false);
}

void SLTimerMgr::schedule(CSLTimerBase* timerBase){
	_timerQueue.push(timerBase);
}
	
void SLTimerMgr::purgeCancelledTimes(){
	std::vector<CSLTimerBase*>& timerContainer = _timerQueue.getContainer();
	auto partItor = std::partition(timerContainer.begin(), timerContainer.end(), IsNotCancelled());
	auto itor = partItor;
	for(; itor != timerContainer.end(); ++itor){
		if( NULL == *itor)
			continue;

		(*itor)->release();
	}

	const int32 iNumPurged = timerContainer.end() - partItor;
	_numPurged -= iNumPurged;

	timerContainer.erase(partItor, timerContainer.end());
	_timerQueue.makeHeap();
}

int64 SLTimerMgr::getNextExp(int64 maxTime){
	int64 now = getJiffies();
	if(_timerQueue.empty() || now > _timerQueue.top()->getExpireTime())
		return 0;
	int32 expireTime = _timerQueue.top()->getExpireTime() - now;
	return maxTime > expireTime ? expireTime : maxTime;
}

}
}
