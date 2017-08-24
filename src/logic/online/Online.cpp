#include "Online.h"
#include "IEventEngine.h"
#include "sltools.h"
#include "EventID.h"

#define ONLINE_TIME_CHECK_INTERVAL 1000

bool Online::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	_self = this;
	return true;
}

bool Online::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_eventEngine, EventEngine);

	_nextMinute = sl::getTimeTickOfNextMin();
	_nextHour = sl::getTimeTickOfNextHour();
	_nowDay = sl::getGameYearDay();
	_nowWeek = sl::getGameYearWeek();
	_nowMonth = sl::getGameMonth();

	START_TIMER(this, 0, TIMER_BEAT_FOREVER, ONLINE_TIME_CHECK_INTERVAL);

	return true;
}

bool Online::destory(sl::api::IKernel * pKernel){
	pKernel->killTimer(this);
	return true;
}

void Online::onTime(sl::api::IKernel* pKernel, int64 timetick){
	if (timetick >= _nextMinute){
		dealMinuteChanged(pKernel, timetick);
		_nextMinute = sl::getTimeTickOfNextMin();
	}

	if (timetick >= _nextHour){
		dealHourChanged(pKernel, timetick);
		_nextHour = sl::getTimeTickOfNextHour();
	}

	if (sl::getGameYearDay() != _nowDay){
		dealDayChanged(pKernel, timetick);
		_nowDay = sl::getGameYearDay();
	}

	if (sl::getGameYearWeek() != _nowWeek){
		_nowWeek = sl::getGameYearWeek();
		dealWeekChanged(pKernel, _nowWeek);
	}

	if (sl::getGameMonth() != _nowMonth){
		_nowMonth = sl::getGameMonth();
		dealMonthChanged(pKernel, _nowMonth);
	}
}


void Online::dealMinuteChanged(sl::api::IKernel* pKernel, int64 timetick){
	logic_event::NewMinute evt;
	evt.tick = timetick;
	evt.time = time(NULL);

	_eventEngine->execEvent(logic_event::EVENT_NEW_MINUTE, &evt, sizeof(evt));
}

void Online::dealHourChanged(sl::api::IKernel* pKernel, int64 timetick){
	logic_event::NewHour evt;
	evt.tick = timetick;
	evt.time = time(NULL);

	_eventEngine->execEvent(logic_event::EVENT_NEW_HOUR, &evt, sizeof(evt));
}

void Online::dealDayChanged(sl::api::IKernel* pKernel, int64 timetick){
	logic_event::NewDay evt;
	evt.tick = timetick;
	evt.time = time(NULL);

	_eventEngine->execEvent(logic_event::EVENT_NEW_DAY, &evt, sizeof(evt));
}

void Online::dealWeekChanged(sl::api::IKernel* pKernel, int32 newWeek){
	logic_event::NewWeek evt;
	evt.week = newWeek;

	_eventEngine->execEvent(logic_event::EVENT_NEW_WEEK, &evt, sizeof(evt));
}

void Online::dealMonthChanged(sl::api::IKernel* pKernel, int32 newMonth){
	logic_event::NewMonth evt;
	evt.month = newMonth;

	_eventEngine->execEvent(logic_event::EVENT_NEW_MONTH, &evt, sizeof(evt));
}



