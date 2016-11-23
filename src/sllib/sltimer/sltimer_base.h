#ifndef SL_SLTIMER_BASE_H
#define SL_SLTIMER_BASE_H
#include "sltimer.h"
#include "slobjectpool.h"
#include "sltype.h"
#include "slbase.h"
#include "sltime_stamp.h"
namespace sl
{
namespace timer
{
class CSLTimerBase: public PoolObject
{
public:
	enum TimerState
	{
		TIME_RECREATE,
		TIME_PAUSED,
		TIME_DESTORY
	};

	CSLTimerBase();

	virtual ~CSLTimerBase() {}

	typedef SLShared_ptr<SmartPoolObject<CSLTimerBase>> SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj();
	static CObjectPool<CSLTimerBase>& ObjPool();
	static CSLTimerBase* createPoolObject();
	static void reclaimPoolObject(CSLTimerBase* obj);
	static void destroyObjPool();
	void onReclaimObject();
	virtual size_t getPoolObjectBytes();

	void setTimerObj(ISLTimer* pTimer) {m_pTimerObj = pTimer;}
	
	TimerState getTimerState() const {return m_stat;}
	void setTimerState(TimerState stat) {m_stat = stat;}

	TimerState updateState();

	void setExpire(uint64 expireStamp){m_expireStamp = expireStamp;}
	uint64 getExpire() const {return m_expireStamp;}

	void onInit();
	void onStart();
	void onTimer();
	void onEnd();

	bool good(){return m_pTimerObj != nullptr;}

private:
	TimerState		m_stat;
	uint64			m_pauseStamp;
	ISLTimer*		m_pTimerObj;
	uint64			m_expireStamp;
	bool			m_bDelay;
	int32			m_iCount;
};
}
}
#endif