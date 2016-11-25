#ifndef SL_SLTIMER_MGR_H
#define SL_SLTIMER_MGR_H
#include "sltimer.h"
#include "sltimer_base.h"
#include "sltime_stamp.h"
#include "slpriority_queue.h"
#include "slsingleton.h"
namespace sl
{
namespace timer
{

class TimersBase
{
public:
	virtual void onCancel() = 0;
};

class Comparator
{
public:
	bool operator()(const CSLTimerBase* a, const CSLTimerBase* b)
	{
		return a->getExpireTime() > b->getExpireTime();
	}
};

class TimersT: public TimersBase, public ISLTimerMgr, public CSingleton<TimersT>
{

public:
	virtual SLTimerHandler SLAPI startTimer(ISLTimer* pTimer, int64 delay, int32 count, int64 interval);
	virtual bool SLAPI killTimer(SLTimerHandler pTimer);
	virtual void SLAPI pauseTimer(SLTimerHandler pTimer);
	virtual void SLAPI resumeTimer(SLTimerHandler pTimer);
	virtual int SLAPI process(uint64 now);

public:
	TimersT();
	virtual ~TimersT();

	inline size_t size() const {return m_TimeQueue.size();}
	inline bool empty() const {return m_TimeQueue.empty();}

	
	bool legal(CSLTimerBase *pTimer) const;

	TimeStamp nextExp(TimeStamp now) const;
	void clear(bool shouldCallCancel = true);

	/*bool getTimerInfo(TimerHandle handle, TimeStamp& time, TimeStamp& interval,
		void*& pUserData) const;*/

private:

	void purgeCanelledTimes();
	void onCancel();
	typedef PriorityQueue<CSLTimerBase*, Comparator> TimerPriorityQueue;
	TimerPriorityQueue		m_TimeQueue;
	CSLTimerBase*			m_pProcessingNode;
	TimeStamp				m_lastProcessTime;
	int						m_iNumCanceled;

	TimersT(const TimersT&);
	TimersT& operator=(const TimersT&);

};
//typedef TimersT<uint32>  Timers;
//typedef TimersT<uint64>  Timers64;

}
}
#endif