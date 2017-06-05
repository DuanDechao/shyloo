#ifndef SL_SLTIMER_MGR_H
#define SL_SLTIMER_MGR_H
#include "sltimer.h"
#include "sltimer_base.h"
#include "sltime_stamp.h"
#include "slsingleton.h"
#include "sltimer_base.h"
namespace sl{
namespace timer{
#define JIFFIES_INTERVAL 20
class TimerGear{
public:
	TimerGear(int32 maxSlotNum, TimerGear* nextGear) 
		:_nowSlot(0),
		 _maxSlotNum(maxSlotNum),
		 _nextGear(nextGear),
		 _slots(nullptr)
	{
		_slots = NEW SLList[maxSlotNum];
	}

	~TimerGear(){
		_nowSlot = 0;
		_maxSlotNum = 0;
		if (_slots){
			DEL[] _slots;
			_slots = nullptr;
		}
	}

	void checkHighGear();

public:
	SLList* _slots;
	TimerGear* _nextGear;
	int32	_nowSlot;
	int32	_maxSlotNum;

private:
	TimerGear(const TimerGear&);
	TimerGear& operator = (const TimerGear&);
};

class SLTimerMgr : public ISLTimerMgr, public SLHolder<SLTimerMgr>{
	enum {
		TQ_GEAR1_BITS = 6,
		TQ_GEAR2_BITS = 8,
		TQ_GEAR1_SIZE = 1 << TQ_GEAR1_BITS,
		TQ_GEAR2_SIZE = 1 << TQ_GEAR2_BITS,
		TQ_GEAR1_MASK = TQ_GEAR1_SIZE - 1,
		TQ_GEAR2_MASK = TQ_GEAR2_SIZE - 1,
	};

public:
	virtual SLTimerHandler SLAPI startTimer(ISLTimer* pTimer, int64 delay, int32 count, int64 interval);
	virtual bool SLAPI killTimer(SLTimerHandler pTimer);
	virtual void SLAPI pauseTimer(SLTimerHandler pTimer);
	virtual void SLAPI resumeTimer(SLTimerHandler pTimer);
	virtual int64 SLAPI process(int64 overTime);

public:
	SLTimerMgr();
	virtual ~SLTimerMgr();

	bool legal(CSLTimerBase *pTimer) const;

	TimeStamp nextExp(TimeStamp now) const;
	void clear(bool shouldCallCancel = true);

	inline jiffies_t getJiffies() { return m_currTimeJiffies; }

private:
	void update();
	void onJiffiesUpdate();
	void reCreateTimer(CSLTimerBase* pTimer);
	void endTimer(CSLTimerBase* pTimer);
	void schedule(CSLTimerBase* timerBase);
	SLList* findTimerList(CSLTimerBase* timerBase);

private:
	CSLTimerBase*			m_pProcessingNode;
	jiffies_t				m_currTimeJiffies;
	int32					m_timerTick;
	SLList					m_runTimerList;
	TimerGear*				m_gear1;
	TimerGear*				m_gear2;
	TimerGear*				m_gear3;
	TimerGear*				m_gear4;
	TimerGear*				m_gear5;

private:
	SLTimerMgr(const SLTimerMgr&);
	SLTimerMgr& operator=(const SLTimerMgr&);

};

}
}
#endif