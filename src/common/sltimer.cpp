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

	void TimerHandle::Cancel()
	{
		if(NULL != m_pTime)
		{
			TimeBase* pTime = m_pTime;
			m_pTime = NULL;
			pTime->Cancel();
		}
	}

	template<class TIME_STAMP>
	TimersT<TIME_STAMP>::~TimersT()
	{
		this->Clear();
	}

	template<class TIME_STAMP>
	TimerHandle TimersT<TIME_STAMP>::Add(TimeStamp startTime, TimeStamp interval, TimerHandler* pHandler, void* pUser)
	{
		Time* pTime = new Time(*this, startTime, interval, pHandler, pUser);
		m_TimeQueue.Push(pTime);
		return TimerHandle(pTime);
	}

	template <class TIME_STAMP>
	void TimersT<TIME_STAMP>::OnCancel()
	{
		++m_iNumCanceled;

		if(m_iNumCanceled * 2 > int(m_TimeQueue.Size()))
		{
			this->PurgeCanelledTimes();
		}
	}

	template<class TIME_STAMP>
	void TimersT<TIME_STAMP>::Clear(bool shouldCallCancel)
	{
		int iMaxLoopCount = (int)m_TimeQueue.Size();
		while(!m_TimeQueue.Empty())
		{
			Time* pTime = m_TimeQueue.UnsafePopBack();
			if(NULL == pTime)
				continue;
			if(!pTime->IsCancelled() && shouldCallCancel)
			{
				--m_iNumCanceled;
				pTime->Cancel();
				if(--iMaxLoopCount == 0)
				{
					shouldCallCancel = false;
				}
			}
			else if(pTime->IsCancelled())
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
			return !pTime->IsCancelled();
		}
	};

	template<class TIME_STAMP>
	void TimersT<TIME_STAMP>::PurgeCanelledTimes()
	{
		TimersT::TimeContainer& stTimeContainer = m_TimeQueue.GetContainer();
		TimersT::TimeContainer::iterator PartIter =
			std::partition(stTimeContainer.begin(), stTimeContainer.end(), IsNotCancelled<Time>());

		TimersT::TimeContainer iter = PartIter;
		for (; iter != stTimeContainer.end(); ++iter)
		{
			if(NULL == *iter)
				continue;
			delete *iter;
		}

		const int iNumPurged = stTimeContainer.end() - PartIter;
		m_iNumCanceled -= iNumPurged;

		stTimeContainer.erase(PartIter, stTimeContainer.end());
		m_TimeQueue.MakeHeap();
	}

	template<class TIME_STAMP>
	int TimersT<TIME_STAMP>::Process(TimeStamp now)
	{
		int numFired = 0;
		while(!(m_TimeQueue.Empty()) && 
			(m_TimeQueue.Top()->GetTime() <= now || m_TimeQueue.Top()->IsCancelled()))
		{
			Time* pTime = m_pProcessingNode = m_TimeQueue.Top();
			m_TimeQueue.Pop();
			if(!pTime->IsCancelled())
			{
				++numFired;
				pTime->TriggerTimer();
			}
			if(!pTime->IsCancelled())
			{
				m_TimeQueue.Push(pTime);
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
		if(m_TimeQueue.Empty() || now > m_TimeQueue.Top()->GetTime())
		{
			return 0;
		}
		return m_TimeQueue.Top()->GetTime() - now;
	}

	template<class TIME_STAMP>
	bool TimersT<TIME_STAMP>::GetTimerInfo(TimerHandle handle, 
										TimeStamp& time,
										TimeStamp& interval, 
										void*& pUserData) const
	{
		Time* pTime = static_cast<Time*>(handle.GetTime());
		if(NULL == pTime)
		{
			return false;
		}
		if(!pTime->IsCancelled())
		{
			time = pTime->GetTime();
			interval = pTime->GetInterval();
			pUser = pTime->GetUserData();
			return true;
		}
		return false;
	}

	inline TimeBase::TimeBase(TimersBase& Owner, TimerHandler* pHandler, void* pUserData)
		:m_Owner(Owner),
		 m_pHandler(pHandler),
		 m_pUserData(pUserData),
		 m_stState(TIME_PENDING)
	{
		SL_ASSERT(pHandler != NULL);
		pHandler->IncTimerRegisterCount();
	}

	inline void TimeBase::Cancel()
	{
		if(this->IsCancelled())
		{
			return;
		}
		SL_ASSERT((m_stState == TIME_PENDING) || (m_stState == TIME_EXECUTING));
		m_stState = TIME_CANCELLED;

		if(m_pHandler)
		{
			m_pHandler->Release(TimerHandle(this), m_pUserData);
			m_pHandler = NULL;
		}
		m_Owner.OnCancel();
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
	void TimersT<TIME_STAMP>::Time::TriggerTimer()
	{
		if(!this->IsCancelled())
		{
			m_stState = TIME_EXECUTING;
			m_pHandler->HandlerTimeOut(TimerHandle(this), m_pUserData);

			if((m_Interval == 0) && !this->IsCancelled())
			{
				this->Cancel();
			}
		}

		if(!this->IsCancelled())
		{
			m_Time += m_Interval;
			m_stState = TIME_PENDING;
		}
	}
}