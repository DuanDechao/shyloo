#ifndef SL_OBJECT_FSM_H
#define SL_OBJECT_FSM_H
#include "slmulti_sys.h"
#include "slcallback.h"
#include <list>
#include "slikernel.h"
using namespace sl::api;
class IObject;
class IObjectMgr;
typedef std::function<void(IKernel*, IObject*, const void* pContext, const int32 size)> StatusCallBack;
typedef std::function<bool(IKernel*, IObject*, const void* pContext, const int32 size)> StatusJudgeCallBack;

class ObjectFSM{
	typedef sl::CallBackType<int32, StatusCallBack>::type		STATUS_CB_POOL;
	typedef sl::CallBackType<int32, StatusJudgeCallBack>::type	STATUS_JUDGE_CB_POOL;
	typedef sl::CallBackType<int64, StatusCallBack>::type		STATUS_CHANGE_CB_POOL;
	typedef sl::CallBackType<int64, StatusJudgeCallBack>::type	STATUS_CHANGE_JUDGE_CB_POOL;

public:
	ObjectFSM();
	~ObjectFSM();

	void RgsEntryJudgeCB(int32 status, const StatusJudgeCallBack& f, const char* debug) { _entryJudgeCBPool.Register(status, f, debug); }
	void RgsChangeJudgeCB(int32 from, int32 to, const StatusJudgeCallBack& f, const char* debug) { _changeJudgeCBPool.Register( ((((int64)from) << 32) | (int64)to), f, debug); }
	void RgsLeaveJudgeCB(int32 status, const StatusJudgeCallBack& f, const char* debug) { _leaveJudgeCBPool.Register(status, f, debug); }

	void RgsEntryStatusCB(int32 status, const StatusCallBack& f, const char* debug) { _entryStatusCBPool.Register(status, f, debug); }
	void RgsChangeStatusCB(int32 from, int32 to, const StatusCallBack& f, const char* debug) { _changeStatusCBPool.Register( ((((int64)from) << 32) | (int64)to), f, debug); }
	void RgsLeaveStatusCB(int32 status, const StatusCallBack& f, const char* debug) { _leaveStatusCBPool.Register(status, f, debug); }

	bool EntryStatus(IKernel* pKernel, IObject* object, int32 status, const void* context, const int32 size);

private:
	int32	_status;
	STATUS_CB_POOL	_entryStatusCBPool;
	STATUS_CHANGE_CB_POOL	_changeStatusCBPool;
	STATUS_CB_POOL	_leaveStatusCBPool;

	STATUS_JUDGE_CB_POOL	_entryJudgeCBPool;
	STATUS_CHANGE_JUDGE_CB_POOL	  _changeJudgeCBPool;
	STATUS_JUDGE_CB_POOL	_leaveJudgeCBPool;
};
#endif