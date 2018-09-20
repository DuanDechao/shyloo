#ifndef SL_SLTIMER_BASE_H
#define SL_SLTIMER_BASE_H
#include "slmulti_sys.h"
#include "sltimer.h"
#include "slpool.h"
#include "sllist.h"

namespace sl{
namespace timer{
typedef uint64 jiffies_t;
class CSLTimerBase{
public:
	enum TimerState{
		TIME_RECREATE = 1,
		TIME_REMOVE,
		TIME_DESTORY,
		TIME_PAUSED
	};

	CSLTimerBase(ISLTimer* pTimer, jiffies_t delay, int32 count, jiffies_t interval);
	CSLTimerBase(CSLTimerBase* pTimerBase){}
	virtual ~CSLTimerBase();

	TimerState getTimerState() const {return _stat;}
	void setTimerState(TimerState stat) {_stat = stat;}

	TimerState pollTimer();

	void setExpireTime(jiffies_t expire){ _expire = expire; }
	jiffies_t getExpireTime() const { return _expire; }
	void adjustExpireTime(jiffies_t nowJiffies);

	void setPauseTime(jiffies_t pause){ _pause = pause; }
	jiffies_t getPauseTime() const { return _pause; }

	jiffies_t getIntervalTime() const { return _interval; }

	void onInit();
	void onStart();
	void onTimer();
	void onPause();
	void onResume();
	void onEnd(bool beForced);

	bool good(){return _pTimerObj != nullptr;}

	bool isDestoryed() const {return _stat == TIME_DESTORY;}
	bool isPaused() const {return _stat == TIME_PAUSED;}
	bool needRecreated() const {return _stat == TIME_RECREATE;}

	static CSLTimerBase* create(ISLTimer* pTimer, int64 delay, int32 count, int64 interval){
		return CREATE_FROM_POOL(s_pool, pTimer, delay, count, interval);
	}
	
	static CSLTimerBase* create(CSLTimerBase* pTimerBase){
		return CREATE_FROM_POOL(s_pool, pTimerBase);
	}

	inline void setDebug(const char* debug) { SafeSprintf(_debug, sizeof(_debug), debug, strlen(debug) + 1); }

	inline void release() {s_pool.recover(this); }

private:
	CSLTimerBase(const CSLTimerBase&);
	CSLTimerBase& operator=(const CSLTimerBase&);

private:
	TimerState		_stat;
	jiffies_t		_pause;
	ISLTimer*		_pTimerObj;
	jiffies_t		_expire;
	bool			_bDelay;
	int32			_iCount;
	jiffies_t		_interval;
	char			_debug[128];
	static sl::SLPool<CSLTimerBase> s_pool;
};

}
}
#endif
