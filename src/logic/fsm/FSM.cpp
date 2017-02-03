#include "EventEngine.h"

sl::api::IKernel* EventEngine::s_kernel = nullptr;
sl::CallBackType<int32, EventCB>::type EventEngine::s_events;
sl::CallBackType<int32, JudgeCB>::type EventEngine::s_judges;

bool EventEngine::initialize(sl::api::IKernel * pKernel){
	s_kernel = pKernel;
	return true;
}

bool EventEngine::launched(sl::api::IKernel * pKernel){
	return true;
}

bool EventEngine::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void EventEngine::RgsEvent(const int32 eventId, const EventCB& cb, const char* debug){
	s_events.Register(eventId, cb, debug);
}

void EventEngine::ExecEvent(const int32 eventId, const void * context, const int32 size){
	s_events.Call(eventId, s_kernel, context, size);
}

void EventEngine::RgsJudge(const int32 eventId, const JudgeCB& cb, const char* debug){
	s_judges.Register(eventId, cb, debug);
}

bool EventEngine::ExecJudge(const int32 eventId, const void * context, const int32 size){
	return s_judges.Call(eventId, false, s_kernel, context, size);
}


