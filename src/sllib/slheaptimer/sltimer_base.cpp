#include "sltimer_base.h"
#include "sltimer_mgr.h"
#include "sltime.h"

namespace sl
{
namespace timer
{
sl::SLPool<CSLTimerBase> CSLTimerBase::s_pool;
CSLTimerBase::CSLTimerBase(ISLTimer* pTimer, jiffies_t delay, int32 count, jiffies_t interval)
	:_stat(TIME_RECREATE),
	_pause(0),
	_pTimerObj(pTimer),
	_expire(delay),
	_bDelay(true),
	_iCount(count),
	_interval(interval)
{}

CSLTimerBase::~CSLTimerBase(){
	_stat = TIME_DESTORY;
	_pause = 0;
	_pTimerObj = nullptr;
	_expire = 0;
	_bDelay = true;
	_iCount = 0;
	_interval = 0;
}

void CSLTimerBase::adjustExpireTime(jiffies_t nowJiffies){
	int64 live = _expire - nowJiffies;
	if (live < 0 && _interval > 0 && (uint64)abs(live) > _interval){
		_expire += (uint64)abs(live) / _interval;
	}
}

CSLTimerBase::TimerState CSLTimerBase::pollTimer(){
	if (!good())
		return TimerState::TIME_DESTORY;

	if(_bDelay){
		onStart();

		if (_stat == TimerState::TIME_DESTORY)
			return  TimerState::TIME_DESTORY;

		return TimerState::TIME_RECREATE;
	}
	else{
		if (_iCount != 0)
			onTimer();

		if (_stat == TimerState::TIME_DESTORY)
			return  TimerState::TIME_DESTORY;

		if(_iCount > 0)
			--_iCount;

		if(_iCount == 0)
			return TimerState::TIME_REMOVE;

		return TimerState::TIME_RECREATE;
	}
}

void CSLTimerBase::onInit(){
	_pTimerObj->onInit((int64)sl::getTimeMilliSecond());
}

void CSLTimerBase::onStart(){
	_pTimerObj->onStart((int64)sl::getTimeMilliSecond());
	_bDelay = false;
}

void CSLTimerBase::onTimer(){
	_pTimerObj->onTime((int64)sl::getTimeMilliSecond());
}

void CSLTimerBase::onPause(){
	_pTimerObj->onPause((int64)sl::getTimeMilliSecond());
}

void CSLTimerBase::onResume(){
	_pTimerObj->onResume((int64)sl::getTimeMilliSecond());
}

void CSLTimerBase::onEnd(bool beForced){
	setTimerState(TimerState::TIME_DESTORY);
	_pTimerObj->onTerminate(beForced, (int64)sl::getTimeMilliSecond());

	if (!beForced)
		release();
}

}
}
