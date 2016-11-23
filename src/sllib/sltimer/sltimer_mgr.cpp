#include "sltimer_mgr.h"
namespace sl
{
namespace timer
{


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


ISLTimer* TimersT::startTimer(int64 delay, int32 count, int64 interval)
{
	CSLTimerBase* pTimerBase = CSLTimerBase::createPoolObject();
	if(nullptr == pTimerBase)
		return false;

	pTimerBase->initialize(pTimer, delay, count, interval);

	if(!pTimerBase->good())
		return false;

	pTimerBase->onInit();
}

bool TimersT::killTimer(ISLTimer* pTimer)
{
	
}

void TimersT::add(CSLTimerBase* pTimerBase)
{
	Timer* pTimer = new Timer(*this, pTimerBase);
	m_TimeQueue.push(pTimer);
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
		Timer* pTimer = m_TimeQueue.unsafePopBack();
		if(nullptr == pTimer)
			continue;
		if(!pTimer->isCancelled() && shouldCallCancel)
		{
			--m_iNumCanceled;
			pTime->cancel();
			if(--iMaxLoopCount == 0)
			{
				shouldCallCancel = false;
			}
		}
		else if(pTime->isCancelled())
		{
			--m_iNumCanceled;
		}
		delete pTime;
	}
	m_iNumCanceled = 0;
	m_TimeQueue = PriorityQueue();
}

template <class TIME>
class IsNotCancelled
{
public:
	bool operator()(const TIME* pTime)
	{
		return !pTime->isCancelled();
	}
};

template<class TIME_STAMP>
void TimersT<TIME_STAMP>::purgeCanelledTimes()
{
	typename PriorityQueue::Container& stTimeContainer = m_TimeQueue.getContainer();
	typename PriorityQueue::Container::iterator PartIter =
		std::partition(stTimeContainer.begin(), stTimeContainer.end(), IsNotCancelled<Time>());

	typename PriorityQueue::Container::iterator iter = PartIter;
	for (; iter != stTimeContainer.end(); ++iter)
	{
		if(NULL == *iter)
			continue;
		delete *iter;
	}

	const int iNumPurged = stTimeContainer.end() - PartIter;
	m_iNumCanceled -= iNumPurged;

	stTimeContainer.erase(PartIter, stTimeContainer.end());
	m_TimeQueue.makeHeap();
}

int TimersT::process(TimeStamp now)
{
	int numFired = 0;
	while(!(m_TimeQueue.empty()) && 
		(m_TimeQueue.top()->getExpireTime() <= now || m_TimeQueue.top()->isCancelled()))
	{
		Timer* pTimer = m_pProcessingNode = m_TimeQueue.top();
		m_TimeQueue.pop();

		++numFired;
		pTimer->getTimerState(pTimer->pollTimer());

		if(pTimer->getTimerState() == CSLTimerBase::TimerState::TIME_RECREATE)
		{
			
		}
		if(pTimer->getTimerState() == CSLTimerBase::TimerState::TIME_PAUSED)
		{
			m_TimeQueue.push(pTime);
		}

		if(pTimer->getTimerState() == CSLTimerBase::TimerState::TIME_DESTORY)
		{
			pTimer->release();
			--m_iNumCanceled;
		}
		
		
	}
	m_pProcessingNode = NULL;
	m_lastProcessTime = now;
	return numFired;
}


template<class TIME_STAMP>
bool TimersT<TIME_STAMP>::legal(TimerHandle handle) const
{
	Time* pTime = static_cast<Time*>(handle.GetTime());
	if(NULL == pTime)
	{
		return false;
	}
	if(pTime == m_pProcessingNode)
	{
		return true;
	}
	for (size_t i = 0; i < m_TimeQueue.Size(); i++)
	{
		if(pTime == m_TimeQueue[i])
		{
			return true;
		}
	}
	return false;
}

template<class TIME_STAMP>
TIME_STAMP TimersT<TIME_STAMP>::nextExp(TimeStamp now) const
{
	if(m_TimeQueue.empty() || now > m_TimeQueue.top()->getTime())
	{
		return 0;
	}
	return m_TimeQueue.top()->getTime() - now;
}

template<class TIME_STAMP>
bool TimersT<TIME_STAMP>::getTimerInfo(TimerHandle handle, 
	TimeStamp& time,
	TimeStamp& interval, 
	void*& pUserData) const
{
	Time* pTime = static_cast<Time*>(handle.getTime());
	if(NULL == pTime)
	{
		return false;
	}
	if(!pTime->isCancelled())
	{
		time = pTime->getTime();
		interval = pTime->getInterval();
		pUser = pTime->getUserData();
		return true;
	}
	return false;
}

template<class TIME_STAMP>
TimersT<TIME_STAMP>::Time::Time(TimersBase& owner, 
	TimeStamp startTime, TimeStamp interval, 
	TimerHandler* pHandler, void* pUserData)
	:TimeBase(owner, pHandler, pUserData),
	m_Time(startTime),
	m_Interval(interval)
{}

template<class TIME_STAMP>
void TimersT<TIME_STAMP>::Time::triggerTimer()
{
	if(!this->isCancelled())
	{
		m_stState = TIME_EXECUTING;
		m_pHandler->handlerTimeOut(TimerHandle(this), m_pUserData);

		if((m_Interval == 0) && !this->isCancelled())
		{
			this->cancel();
		}
	}

	if(!this->isCancelled())
	{
		m_Time += m_Interval;
		m_stState = TIME_PENDING;
	}
}
}
}