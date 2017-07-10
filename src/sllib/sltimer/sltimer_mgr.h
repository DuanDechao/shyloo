#ifndef SL_SLTIMER_MGR_H
#define SL_SLTIMER_MGR_H
#include "sltimer.h"
#include "sltimer_base.h"
#include "slsingleton.h"
#include "sltimer_base.h"
namespace sl{
namespace timer{
#define JIFFIES_INTERVAL 20
class SLTimerGear;
class SLTimerMgr : public ISLTimerMgr, public CSingleton<SLTimerMgr>{
	enum {
		TQ_GEAR1_BITS = 6,
		TQ_GEAR2_BITS = 8,
		TQ_GEAR1_SIZE = 1 << TQ_GEAR1_BITS,
		TQ_GEAR2_SIZE = 1 << TQ_GEAR2_BITS,
		TQ_GEAR1_MASK = TQ_GEAR1_SIZE - 1,
		TQ_GEAR2_MASK = TQ_GEAR2_SIZE - 1,
	};

public:
	virtual SLTimerHandler SLAPI startTimer(ISLTimer* pTimer, int64 delay, int32 count, int64 interval, const char* debug);
	virtual bool SLAPI killTimer(SLTimerHandler pTimer);
	virtual void SLAPI pauseTimer(SLTimerHandler pTimer);
	virtual void SLAPI resumeTimer(SLTimerHandler pTimer);
	virtual int64 SLAPI process(int64 overTime);
	virtual void SLAPI release();

public:
	SLTimerMgr();
	virtual ~SLTimerMgr();

	inline jiffies_t getJiffies() { return m_currTimeJiffies; }
	void schedule(CSLTimerBase* timerBase);
	void moveToRunList(CSLTimerBase* timerBase) { m_runTimerList.pushBack(timerBase); }

private:
	void update();
	void onJiffiesUpdate();
	void reCreateTimer(CSLTimerBase* pTimer);
	void endTimer(CSLTimerBase* pTimer);
	SLList* findTimerList(CSLTimerBase* timerBase);

private:
	jiffies_t				m_currTimeJiffies;
	int32					m_timerTick;
	SLList					m_runTimerList;
	SLTimerGear*			m_gear1;
	SLTimerGear*			m_gear2;
	SLTimerGear*			m_gear3;
	SLTimerGear*			m_gear4;
	SLTimerGear*			m_gear5;

private:
	SLTimerMgr(const SLTimerMgr&);
	SLTimerMgr& operator=(const SLTimerMgr&);

};

}
}
#endif