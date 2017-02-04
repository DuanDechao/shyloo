#ifndef _SL_IDBMGR_H__
#define _SL_IDBMGR_H__
#include "slimodule.h"
#define INVAILD_CB_ID -1

class OArgs;
typedef std::function<void(sl::api::IKernel*, const sl::api::ICacheDataResult&)> DBTaskCallBackType;
class IDB : public sl::api::IModule
{
public:
	virtual ~IDB() {}
	virtual void execDBTask(sl::api::IDBTask* pTask, const OArgs& args, DBTaskCallBackType cb = nullptr) = 0;
};

#define CALL_DB_BACK(_db, _task, _params, _cb) _db->execDBTask(_task, _params, std::bind(&_cb, this, std::placeholders::_1,std::placeholders::_2))
#define CALL_DB(_db, _task, _params) _db->execDBTask(_task, _params)
#endif