#ifndef SL_INTERFACE_EVENT_ENGINE_H
#define SL_INTERFACE_EVENT_ENGINE_H
#include "slimodule.h"
class IKernel;
class IObject;
typedef std::function<void(IKernel*, IObject*, const void* context, const int32 size)> StatusCallBack;
typedef std::function<bool(IKernel*, IObject*, const void* context, const int32 size)> StatusJudgeCallBack;

class IFSM: public sl::api::IModule
{
public:
	virtual ~IFSM(){}

	virtual void RgsEntryJudgeCB(IObject* object, int32 status, const StatusJudgeCallBack& f, const char* debug) = 0;
	virtual void RgsChangeJudgeCB(IObject* object, int32 from, int32 to, const StatusJudgeCallBack& f, const char* debug) = 0;
	virtual void RgsLeaveJudgeCB(IObject* object, int32 status, const StatusJudgeCallBack& f, const char* debug) = 0;

	virtual void RgsEntryCB(IObject* object, int32 status, const StatusCallBack& f, const char* debug) = 0;
	virtual void RgsChangeCB(IObject* object, int32 from, int32 to, const StatusCallBack& f, const char* debug) = 0;
	virtual void RgsLeaveCB(IObject* object, int32 status, const StatusCallBack& f, const char* debug) = 0;

	virtual void EntryStatus(IObject* object, int32 status, const void* context, const int32 size) = 0;
};

#define RGS_ENTRY_JUDGE(fsm, obj, status, cb) fsm->RgsEntryJudgeCB(obj, status, std::bind(&cb, this, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, #cb)
#define RGS_CHANGE_JUDGE(fsm, obj, from, to, cb) fsm->RgsChangeJudgeCB(obj, from, to, std::bind(&cb, this, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, #cb)
#define RGS_LEAVE_JUDGE(fsm, obj, status, cb) fsm->RgsLeaveJudgeCB(obj, status, std::bind(&cb, this, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, #cb)

#define RGS_ENTRY_STATUS(fsm, obj, status, cb) fsm->RgsEntryCB(obj, status, std::bind(&cb, this, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, #cb)
#define RGS_CHANGE_STATUS(fsm, obj, from, to, cb) fsm->RgsChangeCB(obj, from, to, std::bind(&cb, this, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, #cb)
#define RGS_LEAVE_STATUS(fsm, obj, status, cb) fsm->RgsLeaveCB(obj, status, std::bind(&cb, this, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, #cb)
#endif