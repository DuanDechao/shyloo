#ifndef __SL_CORE_EVENT_ENGINE_H__
#define __SL_CORE_EVENT_ENGINE_H__
#include "slikernel.h"
#include "IEventEngine.h"
#include "slcallback.h"
#include "slsingleton.h"
class EventEngine :public IEventEngine, public sl::SLHolder<EventEngine>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void rgsEvent(const int32 eventId, const EventCB& cb, const char* debug);
	virtual void execEvent(const int32 eventId, const void * context, const int32 size);

	virtual void rgsJudge(const int32 eventId, const JudgeCB& cb, const char* debug);
	virtual bool execJudge(const int32 eventId, const void * context, const int32 size);

private:
	sl::api::IKernel*	_kernel;
	sl::CallBackType<int32, EventCB>::type _events;
	sl::CallBackType<int32, JudgeCB>::type _judges;
};
#endif