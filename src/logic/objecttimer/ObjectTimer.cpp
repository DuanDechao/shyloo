#include "ObjectTimer.h"
#include "OCTimer.h"
bool ObjectTimer::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	_self = this;
	return true;
}

bool ObjectTimer::launched(sl::api::IKernel * pKernel){
	return true;
}

bool ObjectTimer::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void ObjectTimer::startTimer(IObject* object, const IProp* prop, int64 delay, int32 count, int64 interval, const object_timer::START_FUNC_TYPE& start,
	const object_timer::ONTIME_FUNC_TYPE& onTime, const object_timer::TERMINATE_FUNC_TYPE& terminate, const char* file, const int32 line){
	
	OCTimer* old = (OCTimer* )object->getTempInt64(prop);
	if (old){
		SLASSERT(false, "timer is already exist");
		_kernel->killTimer(old);
	}

	OCTimer* newTimer = OCTimer::create(_kernel, object, prop, start, onTime, terminate);
	object->setTempInt64(prop, (int64)newTimer);

	_kernel->startTimer(newTimer, delay, count, interval, file, line);
}

void ObjectTimer::stopTimer(IObject* object, const IProp* prop){
	OCTimer* timer = (OCTimer*)object->getTempInt64(prop);
	if (timer)
		_kernel->killTimer(timer);
}
