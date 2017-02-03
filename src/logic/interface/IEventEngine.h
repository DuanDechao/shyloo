#ifndef SL_INTERFACE_EVENT_ENGINE_H
#define SL_INTERFACE_EVENT_ENGINE_H
#include "slikernel.h"
#include "slimodule.h"

typedef std::function<void(sl::api::IKernel*, const void*, const int32)> EventCB;
typedef std::function<bool(sl::api::IKernel*, const void*, const int32)> JudgeCB;

class IEventEngine: public sl::api::IModule
{
public:
	virtual ~IEventEngine(){}

	virtual void RgsEvent(const int32 eventId, const EventCB& cb, const char* debug) = 0;
	virtual void ExecEvent(const int32 eventId, const void * context, const int32 size) = 0;

	virtual void RgsJudge(const int32 eventId, const JudgeCB& cb, const char* debug) = 0;
	virtual bool ExecJudge(const int32 eventId, const void * context, const int32 size) = 0;
};

#define RGS_EVENT_HANDLER(eventEngine, eventId, cb) eventEngine->RgsEvent(eventId, std::bind(&cb, this, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3), #cb)
#define RGS_JUDGE_HANDLER(eventEngine, eventId, cb) eventEngine->RgsJudge(eventId, std::bind(&cb, this, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3), #cb)

#endif