#ifndef _SL_IDBMGR_H__
#define _SL_IDBMGR_H__
#include "slimodule.h"
class OArgs;
class IDB : public sl::api::IModule
{
public:
	virtual ~IDB() {}
	virtual void execDBTask(sl::api::IDBTask* pTask, int32 cbID, const OArgs& args) = 0;
	virtual void rgsDBTaskCallBack(int32 messageId, sl::api::ICacheDataResult::DataReadFuncType handler) = 0;
};

#endif