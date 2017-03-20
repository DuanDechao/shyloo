#define SL_DLL_EXPORT
#include "sltimer_mgr.h"
namespace sl
{
SL_SINGLETON_INIT(timer::TimersT);

namespace timer
{
extern "C" SL_DLL_API ISLTimerMgr* SLAPI getSLTimerModule()
{
	TimersT* g_timersPtr = TimersT::getSingletonPtr();
	if(g_timersPtr == NULL)
		g_timersPtr = new TimersT();
	return TimersT::getSingletonPtr();
}

TimersT::TimersT()
	:m_TimeQueue(),
	m_lastProcessTime(0),
	m_pProcessingNode(NULL),
	m_iNumCanceled(0)
{}



TimersT::~TimersT()
{
	this->clear();
}


SLTimerHandler TimersT::startTimer(ISLTimer* pTimer, int64 delay, int32 count, int64 interval)
{
	if(nullptr == pTimer)
		return false;

	CSLTimerBase* pMgrTimerObj = CREATE_POOL_OBJECT(CSLTimerBase, this, pTimer, delay, count, interval);
	if(nullptr == pMgrTimerObj)
		return false;
	
	if(!pMgrTimerObj->good())
		return false;

	m_TimeQueue.push(pMgrTimerObj);
	
	pMgrTimerObj->onInit();

	return pMgrTimerObj;
}

bool TimersT::killTimer(SLTimerHandler pTimer)
{
	if(INVALID_TIMER_HANDER == pTimer)
		return false;

	CSLTimerBase* pTimerBase = (CSLTimerBase*)pTimer;
	if(!pTimerBase->good() || !legal(pTimerBase))
		return false;

	pTimerBase->release();
	return true;
}

void TimersT::pauseTimer(SLTimerHandler pTimer)
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

void TimersT::resumeTimer(SLTimerHandler pTimer)
{
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
void TimersT::onCancel()
{
	++m_iNumCanceled;

	if(m_iNumCanceled * 2 > int(m_TimeQueue.size()))
	{
		this->purgeCanelledTimes();
	}
}

void TimersT::clear(bool shouldCallCancel)
{
	int iMaxLoopCount = (int)m_TimeQueue.size();
	while(!m_TimeQueue.empty())
	{
		CSLTimerBase* pTimer = m_TimeQueue.unsafePopBack();
		if(nullptr == pTimer)
			continue;
		if(!pTimer->isDestoryed() && shouldCallCancel)
		{
			--m_iNumCanceled;
			pTimer->release();
			if(--iMaxLoopCount == 0)
			{
				shouldCallCancel = false;
			}
		}
		else if(pTimer->isDestoryed())
		{
			--m_iNumCanceled;
		}
		delete pTimer;
	}
	m_iNumCanceled = 0;
	m_TimeQueue.clear();
}

template <class TIME>
class IsNotCancelled
{
public:
	bool operator()(const TIME* pTime)
	{
		return !pTime->isDestoryed();
	}
};

void TimersT::purgeCanelledTimes()
{
	TimerPriorityQueue::Container& stTimeContainer = m_TimeQueue.getContainer();
	TimerPriorityQueue::Container::iterator PartIter =
		std::partition(stTimeContainer.begin(), stTimeContainer.end(), IsNotCancelled<CSLTimerBase>());

	TimerPriorityQueue::Container::iterator iter = PartIter;
	for (; iter != stTimeContainer.end(); ++iter)
	{
		if(NULL == *iter)
			continue;
		RELEASE_POOL_OBJECT(CSLTimerBase, *iter);
	}

	int32 iNumPurged = (int32)(stTimeContainer.end() - PartIter);
	m_iNumCanceled -= iNumPurged;

	stTimeContainer.erase(PartIter, stTimeContainer.end());
	m_TimeQueue.makeHeap();
}

int64 TimersT::process(int64 overTime)
{
	int64 tick = sl::getTimeMilliSecond();
	int numFired = 0;
	while(!(m_TimeQueue.empty()) && 
		(m_TimeQueue.top()->getExpireTime() <= timestamp() || m_TimeQueue.top()->isDestoryed()))
	{
		if (sl::getTimeMilliSecond() - (uint64)tick > (uint64)overTime)
			break;

		CSLTimerBase* pTimer = m_pProcessingNode = m_TimeQueue.top();
		m_TimeQueue.pop();

		if(pTimer->isDestoryed())
		{
			--m_iNumCanceled;
			RELEASE_POOL_OBJECT(CSLTimerBase, pTimer);
			continue;
		}

		if(pTimer->isPaused())
		{
			continue;
		}

		++numFired;
		pTimer->setTimerState(pTimer->pollTimer());

		if(pTimer->needRecreated())
		{
			pTimer->setExpireTime(timestamp() + pTimer->getIntervalTime());
			m_TimeQueue.push(pTimer);
		}

		if(pTimer->isDestoryed())
		{
			--m_iNumCanceled;
			pTimer->release();
			RELEASE_POOL_OBJECT(CSLTimerBase, pTimer);
		}
		
		
	}
	m_pProcessingNode = NULL;
	m_lastProcessTime = timestamp();
	return numFired;
}


bool TimersT::legal(CSLTimerBase* pTimer) const
{
	if(NULL == pTimer)
	{
		return false;
	}
	if(pTimer == m_pProcessingNode)
	{
		return true;
	}
	for (size_t i = 0; i < m_TimeQueue.size(); i++)
	{
		if(pTimer == m_TimeQueue[i])
		{
			return true;
		}
	}
	return false;
}

TimeStamp TimersT::nextExp(TimeStamp now) const
{
	if(m_TimeQueue.empty() || now > m_TimeQueue.top()->getExpireTime())
	{
		return 0;
	}
	return m_TimeQueue.top()->getExpireTime() - now;
}

}
}