#ifndef _SL_IDBMGR_H__
#define _SL_IDBMGR_H__
#include "slimodule.h"
#define INVAILD_CB_ID -1

class IDBCall{

};

class IDB : public sl::api::IModule{
public:
	virtual ~IDB() {}

	virtual IDBCall* create(int64 threadId, const int64 id, const char* file, const int32 line, const void* context, const int32 size = 0) = 0;
};

#define CREATE_DB_CALL(_db, _task, _params, _cb) _db->execDBTask(_task, _params, std::bind(&_cb, this, std::placeholders::_1,std::placeholders::_2))
#define CREATE_DB_CALL_CONTEXT(_db, _task, _params) _db->execDBTask(_task, _params)
#endif