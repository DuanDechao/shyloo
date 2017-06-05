#define SL_DLL_EXPORT
#include "sltimer_mgr.h"
#include "sltime.h"
namespace sl{
namespace timer{
extern "C" SL_DLL_API ISLTimerMgr* SLAPI getSLTimerModule(){
	SLTimerMgr* g_timersPtr = SLTimerMgr::getInstance();
	return g_timersPtr;
}

SLTimerMgr::SLTimerMgr()
	:m_TimeQueue(),
	m_lastProcessTime(0),
	m_pProcessingNode(NULL),
	m_iNumCanceled(0)
{}



SLTimerMgr::~SLTimerMgr()
{
	this->clear();
}


SLTimerHandler SLTimerMgr::startTimer(ISLTimer* pTimer, int64 delay, int32 count, int64 interval){
	if (interval != 0 && interval < JIFFIES_INTERVAL)
		interval = JIFFIES_INTERVAL;

	if (delay > 0 && delay < JIFFIES_INTERVAL)
		delay = JIFFIES_INTERVAL;

	CSLTimerBase* pMgrTimerObj = CSLTimerBase::create(pTimer, (jiffies_t)(getJiffies() + delay / JIFFIES_INTERVAL), count, (jiffies_t)(interval / JIFFIES_INTERVAL));
	SLASSERT(pMgrTimerObj && pMgrTimerObj->good(), "create timerbase failed");
	
	pMgrTimerObj->onInit();

	schedule(pMgrTimerObj);
	
	return pMgrTimerObj;
}

bool SLTimerMgr::killTimer(SLTimerHandler pTimer){
	if(INVALID_TIMER_HANDER == pTimer)
		return false;

	CSLTimerBase* pTimerBase = (CSLTimerBase*)pTimer;
	if(!pTimerBase->good() || !legal(pTimerBase))
		return false;

	pTimerBase->release();
	return true;
}

void SLTimerMgr::pauseTimer(SLTimerHandler pTimer)
{
	if(INVALID_TIMER_HANDER == pTimer)
		return;

	CSLTimerBase* pTimerBase = (CSLTimerBase*)pTimer;
	if(!pTimerBase->good() || !legal(pTimerBase))
		return;
	pTimerBase->setTimerState(CSLTimerBase::TimerState::TIME_PAUSED);
	pTimerBase->setPauseTime(timestamp());
	pTimerBase->onPause();
}

void SLTimerMgr::resumeTimer(SLTimerHandler pTimer){
	if(INVALID_TIMER_HANDER == pTimer)
		return;

	CSLTimerBase* pTimerBase = (CSLTimerBase*)pTimer;
	if(!pTimerBase->good() || pTimerBase->getTimerState() != CSLTimerBase::TimerState::TIME_PAUSED)
		return;

	pTimerBase->setTimerState(CSLTimerBase::TimerState::TIME_RECREATE);
	pTimerBase->setExpireTime(pTimerBase->getPauseTime() + pTimerBase->getExpireTime() - pTimerBase->getPauseTime());
	m_TimeQueue.push(pTimerBase);
	pTimerBase->onResume();
}

int64 SLTimerMgr::process(int64 overTime){
	static int64 last = sl::getTimeMilliSecond();
	int64 tick = sl::getTimeMilliSecond();

	int32 count = (int32)(tick - last) / 10;
	for (int32 i = 0; i < count; i++){
		update();
	}
	last += count * 10;

	while (true){
		if (sl::getTimeMilliSecond() - tick > overTime)
			break;

		CSLTimerBase* pTimer = (CSLTimerBase*)m_runTimerList.popFront();
		if (!pTimer)
			break;

		CSLTimerBase::TimerState state = pTimer->pollTimer();
		switch (state){
		case sl::timer::CSLTimerBase::TIME_RECREATE: reCreateTimer(pTimer); break;
		case sl::timer::CSLTimerBase::TIME_REMOVE: endTimer(pTimer); break;
		case sl::timer::CSLTimerBase::TIME_PAUSED: break;
		case sl::timer::CSLTimerBase::TIME_DESTORY: pTimer->release(); break;
		}
	}

	return sl::getTimeMilliSecond() - tick;
}

void SLTimerMgr::update(){
	m_timerTick++;
	if (m_timerTick == 2){
		onJiffiesUpdate();
		m_timerTick = 0;
	}
}

void SLTimerMgr::onJiffiesUpdate(){
	if (m_gear1)
		m_gear1->
}

void SLTimerMgr::schedule(CSLTimerBase* timerBase){
	SLList* list = findTimerList(timerBase);
	SLASSERT(list, "wtf");
	if (list)
		list->pushBack(timerBase);
}

SLList* SLTimerMgr::findTimerList(CSLTimerBase* timerBase){
	jiffies_t expireJiffies = timerBase->getExpireTime();
	if (expireJiffies <= m_currTimeJiffies)
		return &m_runTimerList;

	jiffies_t restJiffies = expireJiffies - m_currTimeJiffies;
	SLList* findList = nullptr;
	if (restJiffies < TQ_GEAR2_SIZE){
		findList = m_gear1->_slots + (TQ_GEAR2_MASK & expireJiffies);
	}
	else if (restJiffies < (1 << (TQ_GEAR2_BITS + TQ_GEAR1_BITS))){
		findList = m_gear2->_slots + (TQ_GEAR1_MASK & (expireJiffies >> TQ_GEAR2_BITS));
	}
	else if (restJiffies < (1 << (TQ_GEAR2_BITS + 2 * TQ_GEAR1_BITS))){
		findList = m_gear3->_slots + (TQ_GEAR1_MASK & (expireJiffies >> (TQ_GEAR2_BITS + TQ_GEAR1_BITS)));
	}
	else if (restJiffies < (1 << (TQ_GEAR2_BITS + 3 * TQ_GEAR1_SIZE))){
		findList = m_gear4->_slots + (TQ_GEAR1_MASK & (expireJiffies >> (TQ_GEAR2_BITS + 2 * TQ_GEAR1_BITS)));
	}
	else if (restJiffies < (1 << (TQ_GEAR2_BITS + 4 * TQ_GEAR1_SIZE))){
		findList = m_gear5->_slots + (TQ_GEAR1_MASK & (expireJiffies >> (TQ_GEAR2_BITS + 3 * TQ_GEAR1_BITS)));
	}
	return findList;
}

}
}