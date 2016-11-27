#ifndef SL_SLTIMER_BASE_H
#define SL_SLTIMER_BASE_H
#include "slmulti_sys.h"
#include "sltimer.h"
#include "slobjectpool.h"
#include "sltime_stamp.h"
namespace sl
{
namespace timer
{
class TimersBase;
class CSLTimerBase: public PoolObject
{
private:
	CSLTimerBase(const CSLTimerBase&);
	CSLTimerBase& operator=(const CSLTimerBase&);
public:
	enum TimerState
	{
		TIME_RECREATE,
		TIME_PAUSED,
		TIME_DESTORY
	};
	CSLTimerBase(){}

	CSLTimerBase(TimersBase* owner, ISLTimer* pTimer, int64 delay, int32 count, int64 interval);

	virtual ~CSLTimerBase() {}

	typedef SLShared_ptr<SmartPoolObject<CSLTimerBase>> SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj();
	static CObjectPool<CSLTimerBase>& ObjPool();
	static CSLTimerBase* createPoolObject();
	static void reclaimPoolObject(CSLTimerBase* obj);
	static void destroyObjPool();
	void onReclaimObject();
	virtual size_t getPoolObjectBytes();

	void initialize(TimersBase* owner, ISLTimer* pTimer, int64 delay, int32 count, int64 interval);

	TimerState getTimerState() const {return m_stat;}
	void setTimerState(TimerState stat) {m_stat = stat;}

	TimerState pollTimer();

	void setExpireTime(TimeStamp expireStamp){m_expireStamp = expireStamp;}
	TimeStamp getExpireTime() const {return m_expireStamp;}

	void setPauseTime(TimeStamp pauseStamp){m_pauseStamp = pauseStamp;}
	TimeStamp getPauseTime() const {return m_pauseStamp;}

	TimeStamp getIntervalTime() const {return m_intervalStamp;}

	void onInit();
	void onStart();
	void onTimer();
	void onPause();
	void onResume();
	void onEnd();

	bool good(){return m_pTimerObj != nullptr;}

	bool isDestoryed() const {return m_stat == TIME_DESTORY;}
	bool isPaused() const {return m_stat == TIME_PAUSED;}
	bool needRecreated() const {return m_stat == TIME_RECREATE;}

	void release();

protected:
	TimerState		m_stat;
	TimeStamp		m_pauseStamp;
	ISLTimer*		m_pTimerObj;
	TimeStamp		m_expireStamp;
	bool			m_bDelay;
	//bool			m_bCanceled;
	int32			m_iCount;
	TimeStamp		m_intervalStamp;
	TimersBase*		m_Owner;
};
}
}
#endif