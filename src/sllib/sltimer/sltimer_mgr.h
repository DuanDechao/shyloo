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
	virtual bool SLAPI startTimer(ISLTimer* pTimer, int64 delay, int32 count, int64 interval) = 0;
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

	//TimerHandle add(TimeStamp startTime, TimeStamp interval, TimerHandler* pHandler, void* pUser);
	//virtual startTimer()

private:
	typedef std::vector<TimeBase*> TimeContainer;
	TimeContainer		m_TimeContainer;

	void purgeCanelledTimes();
	void onCancel();

	class Time: public TimeBase
	{
	public:
		Time(TimersBase& owner, TimeStamp startTime, TimeStamp interval,
			TimerHandler* pHandler, void* pUserData);
	private:
		Time(const Time&);
		Time& operator=(const Time&);
	public:
		TimeStamp	getTime() const {return m_Time; }
		TimeStamp	getInterval() const {return m_Interval;}

		void triggerTimer();
	private:
		TimeStamp			m_Time;
		TimeStamp			m_Interval;
	};

	class Comparator
	{
	public:
		bool operator()(const Time* a, const Time* b)
		{
			return a->getTime() > b->getTime();
		}
	};

	class PriorityQueue
	{
	public:
		typedef std::vector<Time*> Container;

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

		Time* unsafePopBack()
		{
			Time* pTime = m_TimeContainer.back();
			m_TimeContainer.pop_back();
			return pTime;
		}

		Container& getContainer() {return m_TimeContainer;}

		void makeHeap()
		{
			std::make_heap(m_TimeContainer.begin(), m_TimeContainer.end(),Comparator());
		}
	private:
		Container		m_TimeContainer;
	};

	PriorityQueue	m_TimeQueue;
	Time*			m_pProcessingNode;
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