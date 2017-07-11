#ifndef _SL_IDBMGR_H__
#define _SL_IDBMGR_H__
#include "slimodule.h"
#include "IMysqlMgr.h"
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

	virtual ~IDBCallCondition(){}

	virtual void AddCondition(const char* key, const DBConditionOpType opt, const int8 value) = 0;
	virtual void AddCondition(const char* key, const DBConditionOpType opt, const int16 value) = 0;
	virtual void AddCondition(const char* key, const DBConditionOpType opt, const int32 value) = 0;
	virtual void AddCondition(const char* key, const DBConditionOpType opt, const int64 value) = 0;
	virtual void AddCondition(const char* key, const DBConditionOpType opt, const char* value) = 0;
};

class IDBQueryParamAdder{
public:
	virtual ~IDBQueryParamAdder(){}

	virtual void AddColumn(const char* key) = 0;
};

class IDBInsertParamAdder{
public:
	virtual ~IDBInsertParamAdder(){}

	virtual void AddColumn(const char* key, const int8 value) = 0;
	virtual void AddColumn(const char* key, const int16 value) = 0;
	virtual void AddColumn(const char* key, const int32 value) = 0;
	virtual void AddColumn(const char* key, const int64 value) = 0;
	virtual void AddColumn(const char* key, const char* value) = 0;
	virtual void AddColumn(const char* key, const void* value, const int32 size) = 0;
};

class IDBUpdateParamAdder{
public:
	virtual ~IDBUpdateParamAdder(){}

	virtual void AddColumn(const char* key, const int8 value) = 0;
	virtual void AddColumn(const char* key, const int16 value) = 0;
	virtual void AddColumn(const char* key, const int32 value) = 0;	
	virtual void AddColumn(const char* key, const int64 value) = 0;
	virtual void AddColumn(const char* key, const char* value) = 0;
	virtual void AddColumn(const char* key, const float value) = 0;
	virtual void AddColumn(const char* key, const void* value, const int32 size) = 0;
};

class IDBSaveParamAdder{
public:
	virtual ~IDBSaveParamAdder(){}

	virtual void AddColumn(const char* key, const int8 value) = 0;
	virtual void AddColumn(const char* key, const int16 value) = 0;
	virtual void AddColumn(const char* key, const int32 value) = 0;
	virtual void AddColumn(const char* key, const int64 value) = 0;
	virtual void AddColumn(const char* key, const char* value) = 0;
	virtual void AddColumn(const char* key, const float value) = 0;
	virtual void AddColumn(const char* key, const void* value, const int32 size) = 0;
};

typedef std::function<void(sl::api::IKernel* pKernel, IDBQueryParamAdder* adder, IDBCallCondition* condition)> DBQueryCommandFunc;
typedef std::function<void(sl::api::IKernel* pKernel, IDBInsertParamAdder* adder)> DBInsertCommandFunc;
typedef std::function<void(sl::api::IKernel* pKernel, IDBUpdateParamAdder* adder, IDBCallCondition* condition)> DBUpdateCommandFunc;
typedef std::function<void(sl::api::IKernel* pKernel, IDBSaveParamAdder* adder)> DBSaveCommandFunc;
typedef std::function<void(sl::api::IKernel* pKernel, IDBCallCondition* condition)> DBDeleteCommandFunc;

class IDBCallSource{
public:
	virtual ~IDBCallSource() {}

	virtual const void* getContext(const int32 size = 0) const = 0;
};

typedef IMysqlResult IDBResult;
typedef std::function<void(sl::api::IKernel* pKernel, const int64 id, const bool success, const int32 affectedRow, const IDBCallSource* source, const IDBResult* result)> DBCallBack;

class IDBCall{
public:
	virtual ~IDBCall() {}

	virtual void query(const char* tableName, const DBQueryCommandFunc& f, const DBCallBack& cb) = 0;
	virtual void insert(const char* tableName, const DBInsertCommandFunc& f, const DBCallBack& cb) = 0;
	virtual void update(const char* tableName, const DBUpdateCommandFunc& f, const DBCallBack& cb) = 0;
	virtual void save(const char* tableName, const DBSaveCommandFunc& f, const DBCallBack& cb) = 0;
	virtual void del(const char* tableName, const DBDeleteCommandFunc& f, const DBCallBack& cb) = 0;
};

class IDB : public sl::api::IModule{
public:
	virtual ~IDB() {}

	virtual IDBCall* create(int64 threadId, const int64 id, const char* file, const int32 line, const void* context, const int32 size = 0) = 0;
};

#define CREATE_DB_CALL(_db, _threadId, _id) _db->create(_threadId, _id, __FILE__, __LINE__, nullptr)
#define CREATE_DB_CALL_CONTEXT(_db, _threadId, _id, _context, _size) _db->create(_threadId, _id, __FILE__, __LINE__, _context, _size)
#define CALLOR_CB(_handler) std::bind(&_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6)
#endif