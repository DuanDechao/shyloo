#define SL_DLL_EXPORT
#include "sltimer_mgr.h"
#include "sltime.h"
#include "sltimer_gear.h" 
namespace sl
{
SL_SINGLETON_INIT(timer::SLTimerMgr);
namespace timer
{

extern "C" SL_DLL_API ISLTimerMgr* SLAPI getSLTimerModule(){
	SLTimerMgr* g_netTimerPtr = SLTimerMgr::getSingletonPtr();
	if (g_netTimerPtr == NULL)
		g_netTimerPtr = NEW SLTimerMgr();
	return SLTimerMgr::getSingletonPtr();
}

SLTimerMgr::SLTimerMgr()
	:m_currTimeJiffies(0),
	 m_timerTick(0),
	 m_gear1(nullptr),
	 m_gear2(nullptr),
	 m_gear3(nullptr),
	 m_gear4(nullptr),
	 m_gear5(nullptr)
{
	m_gear5 = NEW SLTimerGear(TQ_GEAR1_SIZE, nullptr);
	m_gear4 = NEW SLTimerGear(TQ_GEAR1_SIZE, m_gear5);
	m_gear3 = NEW SLTimerGear(TQ_GEAR1_SIZE, m_gear4);
	m_gear2 = NEW SLTimerGear(TQ_GEAR1_SIZE, m_gear3);
	m_gear1 = NEW SLTimerGear(TQ_GEAR2_SIZE, m_gear2);
}

SLTimerMgr::~SLTimerMgr(){
	m_currTimeJiffies = 0;
	m_timerTick = 0;
	if (m_gear1)
		DEL m_gear1;
	if (m_gear2)
		DEL m_gear2;
	if (m_gear3)
		DEL m_gear3;
	if (m_gear4)
		DEL m_gear4;
	if (m_gear5)
		DEL m_gear5;

	m_gear1 = nullptr;
	m_gear2 = nullptr;
	m_gear3 = nullptr;
	m_gear4 = nullptr;
	m_gear5 = nullptr;
}

void SLTimerMgr::release(){
	DEL this;
}


SLTimerHandler SLTimerMgr::startTimer(ISLTimer* pTimer, int64 delay, int32 count, int64 interval, const char* debug){
	if (interval != 0 && interval < JIFFIES_INTERVAL)
		interval = JIFFIES_INTERVAL;

	if (delay > 0 && delay < JIFFIES_INTERVAL)
		delay = JIFFIES_INTERVAL;

	CSLTimerBase* pMgrTimerObj = CSLTimerBase::create(pTimer, (jiffies_t)(getJiffies() + delay / JIFFIES_INTERVAL), count, (jiffies_t)(interval / JIFFIES_INTERVAL));
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

	SLList* list = pTimerBase->getList();
	if (list){
		list->remove(pTimerBase);
	}

	pTimerBase->onEnd(true);

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

	SLList* list = pTimerBase->getList();
	if (list)
		list->remove(pTimerBase);
	
	schedule(pTimerBase);

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
		case sl::timer::CSLTimerBase::TIME_DESTORY: pTimer->release(); break;
		case sl::timer::CSLTimerBase::TIME_PAUSED: break;
		default: break;
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
		m_gear1->checkHighGear();

	++m_currTimeJiffies;

	if (m_gear1)
		m_gear1->update();
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
	if (restJiffies < TQ_GEAR2_SIZE)
		findList = m_gear1->_slots + (TQ_GEAR2_MASK & expireJiffies);
	else if (restJiffies < (1 << (TQ_GEAR2_BITS + TQ_GEAR1_BITS)))
		findList = m_gear2->_slots + (TQ_GEAR1_MASK & (expireJiffies >> TQ_GEAR2_BITS));
	else if (restJiffies < (1 << (TQ_GEAR2_BITS + 2 * TQ_GEAR1_BITS)))
		findList = m_gear3->_slots + (TQ_GEAR1_MASK & (expireJiffies >> (TQ_GEAR2_BITS + TQ_GEAR1_BITS)));
	else if (restJiffies < (1 << (TQ_GEAR2_BITS + 3 * TQ_GEAR1_BITS)))
		findList = m_gear4->_slots + (TQ_GEAR1_MASK & (expireJiffies >> (TQ_GEAR2_BITS + 2 * TQ_GEAR1_BITS)));
	else if ((long long)restJiffies < 0)
		findList = m_gear1->_slots + (TQ_GEAR2_MASK & expireJiffies);
	else 
		findList = m_gear5->_slots + (TQ_GEAR1_MASK & (expireJiffies >> (TQ_GEAR2_BITS + 3 * TQ_GEAR1_BITS)));

	return findList;
}

}
}
