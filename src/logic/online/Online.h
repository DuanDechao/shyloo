#ifndef __SL_FRAMEWORK__ONLINE_H__
#define __SL_FRAMEWORK__ONLINE_H__
#include "slimodule.h"
#include "slikernel.h"

class IEventEngine;
class Online : public sl::api::IModule, public sl::api::ITimer{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}

	void dealMinuteChanged(sl::api::IKernel* pKernel, int64 timetick);
	void dealHourChanged(sl::api::IKernel* pKernel, int64 timetick);
	void dealDayChanged(sl::api::IKernel* pKernel, int64 timetick);
	void dealWeekChanged(sl::api::IKernel* pKernel, int32 newWeek);
	void dealMonthChanged(sl::api::IKernel* pKernel, int32 newMonth);

private:
	Online*				_self;
	sl::api::IKernel*	_kernel;
	IEventEngine*		_eventEngine;

	int64				_nextMinute;
	int64				_nextHour;
	int32				_nowDay;
	int32				_nowWeek;
	int32				_nowMonth;
};
#endif