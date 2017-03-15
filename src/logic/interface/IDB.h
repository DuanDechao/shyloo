#ifndef _SL_IDBMGR_H__
#define _SL_IDBMGR_H__
#include "slimodule.h"
#include <functional>
class IDBCallCondition{
public:
	enum DBConditionOpType{
		DBOP_EQ = 0, //  =
		DBOP_NE,	 // !=
		DBOP_GT,	 // >
		DBOP_GE,	 // >=
		DBOP_LS,	 // <
		DBOP_LE,	 // <=
	};

	virtual void AddCondition(const char* key, const DBConditionOpType opt, const int8 value) = 0;
	virtual void AddCondition(const char* key, const DBConditionOpType opt, const int16 value) = 0;
	virtual void AddCondition(const char* key, const DBConditionOpType opt, const int32 value) = 0;
	virtual void AddCondition(const char* key, const DBConditionOpType opt, const int64 value) = 0;
	virtual void AddCondition(const char* key, const DBConditionOpType opt, const char* value) = 0;
};

class IDBQueryParamAdder{

};

class IDBInsertParamAdder{

};

class IDBUpdateParamAdder{

};

typedef std::function<void(sl::api::IKernel* pKernel, IDBQueryParamAdder* adder, IDBCallCondition* condition)> DBQueryCommandFunc;
typedef std::function<void(sl::api::IKernel* pKernel, IDBInsertParamAdder* adder)> DBInsertCommandFunc;
typedef std::function<void(sl::api::IKernel* pKernel, IDBUpdateParamAdder* adder, IDBCallCondition* condition)> DBUpdateCommandFunc;
typedef std::function<void(sl::api::IKernel* pKernel, IDBCallCondition* condition)> DBDeleteCommandFunc;

class IDBCallSource{
public:
	virtual ~IDBCallSource() {}
};

class IDBResult{
public:
	virtual ~IDBResult() {}
};

typedef std::function<void(sl::api::IKernel* pKernel, const int64 id, const bool success, const int32 affectedRow, const IDBCallSource* source, const IDBResult* result)> DBCallBack;

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