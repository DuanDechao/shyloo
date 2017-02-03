#ifndef __SL_FRAMEWORK_GATE_H__
#define __SL_FRAMEWORK_GATE_H__
#include "slikernel.h"
#include "IEventEngine.h"
#include "slcallback.h"
class EventEngine :public IEventEngine{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void RgsEvent(const int32 eventId, const EventCB& cb, const char* debug);
	virtual void ExecEvent(const int32 eventId, const void * context, const int32 size);

	virtual void RgsJudge(const int32 eventId, const JudgeCB& cb, const char* debug);
	virtual bool ExecJudge(const int32 eventId, const void * context, const int32 size);

private:
	static sl::api::IKernel*	s_kernel;
	static sl::CallBackType<int32, EventCB>::type s_events;
	static sl::CallBackType<int32, JudgeCB>::type s_judges;
};
#endif