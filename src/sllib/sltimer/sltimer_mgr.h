#ifndef SL_SLTIMER_MGR_H
#define SL_SLTIMER_MGR_H
#include "sltimer.h"
#include "sltimer_base.h"
#include "sltime_stamp.h"
namespace sl
{
namespace timer
{

class TimersBase
{
public:
	virtual void onCancel() = 0;
};

class TimersT: public TimersBase, public ISLTimerMgr
{

public:
	virtual bool SLAPI startTimer(int64 delay, int32 count, int64 interval) = 0;
	virtual bool SLAPI killTimer(ISLTimer* pTimer) = 0;
	virtual void SLAPI pauseTimer(ISLTimer* pTimer) = 0;
	virtual void SLAPI resumeTimer(ISLTimer* pTimer) = 0;

public:
	typedef int64 TimeStamp;

	TimersT();
	virtual ~TimersT();

	inline UINT32 size() const {return m_TimeQueue.size();}
	inline bool empty() const {return m_TimeQueue.empty();}

	int process(TimeStamp now);
	bool legal(TimerHandle handle) const;

	TimeStamp nextExp(TimeStamp now) const;
	void clear(bool shouldCallCancel = true);

	bool getTimerInfo(TimerHandle handle, TimeStamp& time, TimeStamp& interval,
		void*& pUserData) const;

	void add(CSLTimerBase* pTimerBase);

private:

	void purgeCanelledTimes();
	void onCancel();

	class Timer
	{
	public:
		Timer(TimersBase& owner, CSLTimerBase* pTimeBase);
	private:
		Timer(const Timer&);
		Timer& operator=(const Timer&);
	public:
		TimeStamp	getExpireTime() const {return m_pTimeBase->getExpire(); }
		CSLTimerBase::TimerState pollTimer() {return m_pTimeBase->updateState();}
		CSLTimerBase::TimerState getTimerState() {return m_pTimeBase->getTimerState();}
		void getTimerState(CSLTimerBase::TimerState stat) {m_pTimeBase->setTimerState(stat);}
		void release() 
		{
			m_pTimeBase->onEnd();
			m_Owner.onCancel();
			m_pTimeBase = nullptr;
			delete this;
		}

		//TimeStamp	getInterval() const {return m_pTimeBase->get;}

		//void triggerTimer();
	private:
		TimersBase&			m_Owner;
		TimeStamp			m_Time;
		//TimeStamp			m_Interval;
		CSLTimerBase*		m_pTimeBase;
	};

	class Comparator
	{
	public:
		bool operator()(const Timer* a, const Timer* b)
		{
			return a->getExpireTime() > b->getExpireTime();
		}
	};

	class PriorityQueue
	{
	public:
		typedef std::vector<Timer*> Container;

		typedef typename Container::value_type value_type;
		typedef typename Container::size_type  size_type;

		bool empty() const {return m_TimeContainer.empty();}
		size_type size() const {return m_TimeContainer.size();}

		const value_type& top() const {return m_TimeContainer.front();}

		void push(const value_type& x)
		{
			m_TimeContainer.push_back(x);
			std::push_heap(m_TimeContainer.begin(), m_TimeContainer.end(), Comparator());
		}

		void pop()
		{
			std::pop_heap(m_TimeContainer.begin(), m_TimeContainer.end(),Comparator());
			m_TimeContainer.pop_back();
		}

		Timer* unsafePopBack()
		{
			Timer* pTimer = m_TimeContainer.back();
			m_TimeContainer.pop_back();
			return pTimer;
		}

		Container& getContainer() {return m_TimeContainer;}

		void makeHeap()
		{
			std::make_heap(m_TimeContainer.begin(), m_TimeContainer.end(),Comparator());
		}
	private:
		Container		m_TimeContainer;
	};

	typedef std::vector<Timer*> TimeContainer;
	TimeContainer		m_TimeContainer;

	PriorityQueue	m_TimeQueue;
	Timer*			m_pProcessingNode;
	TimeStamp		m_lastProcessTime;
	int				m_iNumCanceled;

	TimersT(const TimersT&);
	TimersT& operator=(const TimersT&);

};
//typedef TimersT<uint32>  Timers;
//typedef TimersT<uint64>  Timers64;

}
}
#endif