#ifndef __SL_INTERFACE_OBJECTTIMER_H__
#define __SL_INTERFACE_OBJECTTIMER_H__
#include "slimodule.h"
class IObject;
namespace object_timer{
	typedef std::function<void(sl::api::IKernel* pKernel, IObject* object, int64 tick)> START_FUNC_TYPE;
	typedef std::function<void(sl::api::IKernel* pKernel, IObject* object, int64 tick)> ONTIME_FUNC_TYPE;
	typedef std::function<void(sl::api::IKernel* pKernel, IObject* object, bool, int64 tick)> TERMINATE_FUNC_TYPE;
}

class IProp;
class IObjectTimer : public sl::api::IModule{
public:
	virtual ~IObjectTimer() {}

	virtual void startTimer(IObject* object, const IProp* prop, int64 delay, int32 count, int64 interval, const object_timer::START_FUNC_TYPE& start,
		const object_timer::ONTIME_FUNC_TYPE& onTime, const object_timer::TERMINATE_FUNC_TYPE& terminate, const char* file, const int32 line) = 0;

	virtual void stopTimer(IObject* object, const IProp* prop) = 0;
};

#define START_OBJECT_TIMER(mgr, object, prop, delay, count, interval, startFunc, onTimeFunc, terminateFunc) mgr->startTimer(object, prop, delay, count, interval, std::bind(&startFunc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),std::bind(&onTimeFunc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), std::bind(&terminateFunc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), __FILE__, __LINE__) 
#endif