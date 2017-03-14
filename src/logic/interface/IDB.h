#ifndef _SL_IDBMGR_H__
#define _SL_IDBMGR_H__
#include "slimodule.h"
#include <functional>
class IDBColumnsAdder{

};

class IDBConditionAdder{

};

class IDBValueAdder{

};

typedef std::function<void(sl::api::IKernel* pKernel, IDBColumnsAdder* adder, IDBConditionAdder* condition)> DBQueryCommandFunc;
typedef std::function<void(sl::api::IKernel* pKernel, IDBValueAdder* adder)> DBInsertCommandFunc;
typedef std::function<void(sl::api::IKernel* pKernel, IDBValueAdder* adder, IDBConditionAdder* condition)> DBUpdateCommandFunc;
typedef std::function<void(sl::api::IKernel* pKernel, IDBConditionAdder* condition)> DBDeleteCommandFunc;

class IDBCall{
public:
	virtual ~IDBCall() {}
};

class IDB : public sl::api::IModule{
public:
	virtual ~IDB() {}

	virtual IDBCall* create(int64 threadId, const int64 id, const char* file, const int32 line, const void* context, const int32 size = 0) = 0;
};

#define CREATE_DB_CALL(_db, _task, _params, _cb) _db->execDBTask(_task, _params, std::bind(&_cb, this, std::placeholders::_1,std::placeholders::_2))
#define CREATE_DB_CALL_CONTEXT(_db, _task, _params) _db->execDBTask(_task, _params)
#endif