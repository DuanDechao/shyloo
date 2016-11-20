#include "sltimer.h"
namespace sl
{
	template<class TIME_STAMP>
	TimersT<TIME_STAMP>::TimersT()
		:m_TimeQueue(),
		m_lastProcessTime(0),
		m_pProcessingNode(NULL),
		m_iNumCanceled(0)
	{}

	

	template<class TIME_STAMP>
	TimersT<TIME_STAMP>::~TimersT()
	{
		this->clear();
	}

	template<class TIME_STAMP>
	TimerHandle TimersT<TIME_STAMP>::add(TimeStamp startTime, TimeStamp interval, TimerHandler* pHandler, void* pUser)
	{
		Time* pTime = new Time(*this, startTime, interval, pHandler, pUser);
		m_TimeQueue.push(pTime);
		return TimerHandle(pTime);
	}

	template <class TIME_STAMP>
	void TimersT<TIME_STAMP>::onCancel()
	{
		++m_iNumCanceled;

		if(m_iNumCanceled * 2 > int(m_TimeQueue.size()))
		{
			this->purgeCanelledTimes();
		}
	}

	template<class TIME_STAMP>
	void TimersT<TIME_STAMP>::clear(bool shouldCallCancel)
	{
		int iMaxLoopCount = (int)m_TimeQueue.size();
		while(!m_TimeQueue.empty())
		{
			Time* pTime = m_TimeQueue.unsafePopBack();
			if(NULL == pTime)
				continue;
			if(!pTime->isCancelled() && shouldCallCancel)
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

	template<class TIME_STAMP>
	int TimersT<TIME_STAMP>::process(TimeStamp now)
	{
		int numFired = 0;
		while(!(m_TimeQueue.empty()) && 
			(m_TimeQueue.top()->getTime() <= now || m_TimeQueue.top()->isCancelled()))
		{
			Time* pTime = m_pProcessingNode = m_TimeQueue.top();
			m_TimeQueue.pop();
			if(!pTime->isCancelled())
			{
				++numFired;
				pTime->triggerTimer();
			}
			if(!pTime->isCancelled())
			{
				m_TimeQueue.push(pTime);
			}
			else
			{
				delete pTime;
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