#ifndef __SL_CORE_DB_CALL_H__
#define __SL_CORE_DB_CALL_H__
#include "IDB.h"
#include "IMysqlMgr.h"
class DB;
class DBCall : public IDBCall, public IMysqlHandler, public IDBCallSource{
public:
	DBCall(DB* db, int64 threadId, int64 id) 
		:_db(db),
		 _threadId(threadId),
		 _id(id)
	{}

	virtual void query(const char* tableName, const DBQueryCommandFunc& f, const DBCallBack& cb);
	virtual void insert(const char* tableName, const DBInsertCommandFunc& f, const DBCallBack& cb);
	virtual void update(const char* tableName, const DBUpdateCommandFunc& f, const DBCallBack& cb);
	virtual void del(const char* tableName, const DBDeleteCommandFunc& f, const DBCallBack& cb);

private:
	DB*			_db;
	DBCallBack	_cb;
	int64		_threadId;
	int64		_id;
};

template<int32 maxSize>
class DBContext : public DBCall{
public:
	char _context[maxSize];
	int32 _size;
	int64 _threadId;
	int64 _id;
};
#endif