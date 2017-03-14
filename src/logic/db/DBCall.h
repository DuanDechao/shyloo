#ifndef __SL_CORE_DB_CALL_H__
#define __SL_CORE_DB_CALL_H__
#include "IDB.h"
#include "IMysqlMgr.h"
class DB;
class DBCall : public IDBCall{
public:
	DBCall(DB* db) :_db(db){}

	virtual void query(const char* tableName, const DBQueryCommandFunc& f);
	virtual void insert(const char* tableName, const DBInsertCommandFunc& f);
	virtual void update(const char* tableName, const DBUpdateCommandFunc& f);
	virtual void del(const char* tableName, const DBDeleteCommandFunc& f);
private:
	SQLCommand& _sqlCommand;
	DB*			_db;
};

template<int32 maxSize>
class DBContext : public DBCall, public IMysqlHandler{
public:
	char _context[maxSize];
	int32 _size;
	int64 _threadId;
	int64 _id;
};
#endif