#ifndef __SL_FRAMEWORK__OBJECTTIMER_H__
#define __SL_FRAMEWORK__OBJECTTIMER_H__
#include "IObjectTimer.h"
#include "slsingleton.h"
class IProp;
class ObjectTimer : public IObjectTimer, public sl::SLHolder<ObjectTimer>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void startTimer(IObject* object, const IProp* prop, int64 delay, int32 count, int64 interval, const object_timer::START_FUNC_TYPE& start,
		const object_timer::ONTIME_FUNC_TYPE& onTime, const object_timer::TERMINATE_FUNC_TYPE& terminate, const char* file, const int32 line);

	virtual void stopTimer(IObject* object, const IProp* prop);

private:
	ObjectTimer* _self;
	sl::api::IKernel* _kernel;

};
#endif