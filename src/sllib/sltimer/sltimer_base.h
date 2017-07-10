#ifndef SL_SLTIMER_BASE_H
#define SL_SLTIMER_BASE_H
#include "slmulti_sys.h"
#include "sltimer.h"
#include "slpool.h"
#include "sllist.h"

namespace sl{
namespace timer{
typedef uint64 jiffies_t;
class CSLTimerBase: public sl::ISLListNode{
public:
	enum TimerState{
		TIME_RECREATE = 1,
		TIME_REMOVE,
		TIME_DESTORY,
		TIME_PAUSED
	};

	CSLTimerBase(ISLTimer* pTimer, jiffies_t delay, int32 count, jiffies_t interval);
	virtual ~CSLTimerBase();

	TimerState getTimerState() const {return m_stat;}
	void setTimerState(TimerState stat) {m_stat = stat;}

	TimerState pollTimer();

	void setExpireTime(jiffies_t expire){ m_expire = expire; }
	jiffies_t getExpireTime() const { return m_expire; }
	void adjustExpireTime(jiffies_t nowJiffies);

	void setPauseTime(jiffies_t pause){ m_pause = pause; }
	jiffies_t getPauseTime() const { return m_pause; }

	jiffies_t getIntervalTime() const { return m_interval; }

	void onInit();
	void onStart();
	void onTimer();
	void onPause();
	void onResume();
	void onEnd(bool beForced);

	bool good(){return m_pTimerObj != nullptr;}

	bool isDestoryed() const {return m_stat == TIME_DESTORY;}
	bool isPaused() const {return m_stat == TIME_PAUSED;}
	bool needRecreated() const {return m_stat == TIME_RECREATE;}

	static CSLTimerBase* create(ISLTimer* pTimer, int64 delay, int32 count, int64 interval){
		return CREATE_FROM_POOL(s_pool, pTimer, delay, count, interval);
	}

	inline void setDebug(const char* debug) { SafeSprintf(m_debug, sizeof(m_debug), debug, strlen(debug) + 1); }

	inline void release() {s_pool.recover(this); }

private:
	CSLTimerBase(const CSLTimerBase&);
	CSLTimerBase& operator=(const CSLTimerBase&);

private:
	TimerState		m_stat;
	jiffies_t		m_pause;
	ISLTimer*		m_pTimerObj;
	jiffies_t		m_expire;
	bool			m_bDelay;
	int32			m_iCount;
	jiffies_t		m_interval;
	int32			m_recursion;
	char			m_debug[128];
	static sl::SLPool<CSLTimerBase> s_pool;
};

}
}
#endif