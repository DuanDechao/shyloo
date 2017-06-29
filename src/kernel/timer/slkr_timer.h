#ifndef SL_KERNEL_TIMER_H
#define SL_KERNEL_TIMER_H
#include "slikernel.h"
#include "sltimer.h"
#include "slpool.h"

namespace sl{
namespace core{
class CKrTimer: public timer::ISLTimer, public api::ITimerBase{
public:
	inline static CKrTimer* create(api::ITimer* pITimer){
		return CREATE_FROM_POOL(s_pool, pITimer);
	}

	inline void release(){
		s_pool.recover(this);
	}

	virtual void SLAPI onInit(int64 timetick);
	virtual void SLAPI onStart(int64 timetick);
	virtual void SLAPI onTime(int64 timetick);
	virtual void SLAPI onTerminate(bool beForced, int64 timetick);
	virtual void SLAPI onPause(int64 timetick);
	virtual void SLAPI onResume(int64 timetick);

	inline void setITimer(api::ITimer* pITimer){ _timer = pITimer; }
	inline void setTimerHandler(timer::SLTimerHandler timerHander) { _timerHander = timerHander;}
	inline timer::SLTimerHandler getTimerHandler() const { return _timerHander; }

private:
	friend sl::SLPool<CKrTimer>;

	CKrTimer(api::ITimer* pITimer) :_timer(pITimer), _timerHander(nullptr){}
	virtual ~CKrTimer(){
		_timer = nullptr;
		_timerHander = INVALID_TIMER_HANDER;
	}

private:
	api::ITimer*				_timer;
	timer::SLTimerHandler		_timerHander;
	static sl::SLPool<CKrTimer> s_pool;
};

}

}

#endif