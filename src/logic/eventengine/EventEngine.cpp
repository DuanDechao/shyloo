#include "EventEngine.h"

bool EventEngine::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	return true;
}

bool EventEngine::launched(sl::api::IKernel * pKernel){
	return true;
}

bool EventEngine::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void EventEngine::rgsEvent(const int32 eventId, const EventCB& cb, const char* debug){
	_events.Register(eventId, cb, debug);
}

void EventEngine::execEvent(const int32 eventId, const void * context, const int32 size){
	_events.Call(eventId, _kernel, context, size);
}

void EventEngine::rgsJudge(const int32 eventId, const JudgeCB& cb, const char* debug){
	_judges.Register(eventId, cb, debug);
}

bool EventEngine::execJudge(const int32 eventId, const void * context, const int32 size){
	return _judges.Call(eventId, false, _kernel, context, size);
}


