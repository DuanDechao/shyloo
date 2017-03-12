#ifndef __SL_FRAMEWORK_GATE_H__
#define __SL_FRAMEWORK_GATE_H__
#include "slikernel.h"
#include "slcallback.h"
#include "IFSM.h"
class FSM :public IFSM{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void RgsEntryJudgeCB(IObject* object, int32 status, const StatusJudgeCallBack& f, const char* debug);
	virtual void RgsChangeJudgeCB(IObject* object, int32 from, int32 to, const StatusJudgeCallBack& f, const char* debug);
	virtual void RgsLeaveJudgeCB(IObject* object, int32 status, const StatusJudgeCallBack& f, const char* debug);

	virtual void RgsEntryCB(IObject* object, int32 status, const StatusCallBack& f, const char* debug);
	virtual void RgsChangeCB(IObject* object, int32 from, int32 to, const StatusCallBack& f, const char* debug);
	virtual void RgsLeaveCB(IObject* object, int32 status, const StatusCallBack& f, const char* debug);

	virtual void EntryStatus(IObject* object, int32 status, const void* context, const int32 size);

private:
	static sl::api::IKernel*	s_kernel;
};
#endif