#ifndef _SL_IDBMGR_H__
#define _SL_IDBMGR_H__
#include "slimodule.h"
#define INVAILD_CB_ID -1

class OArgs;
class IDB : public sl::api::IModule
{
public:
	virtual ~IDB() {}
	virtual void execDBTask(sl::api::IDBTask* pTask, const OArgs& args, int32 cbID = INVAILD_CB_ID) = 0;
	virtual void rgsDBTaskCallBack(int32 messageId, sl::api::ICacheDataResult::DataReadFuncType handler) = 0;
};

#endif