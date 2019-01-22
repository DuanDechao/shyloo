#ifndef __SL_CORE_DB_CALL_H__
#define __SL_CORE_DB_CALL_H__
#include "IDB.h"
#include "IMysqlMgr.h"
#include "sltools.h"
#include "slpool.h"

class DB;
class DBCall : public IDBCall, public IDBCallSource{
public:
	DBCall(IDBInterface* pdbi, int64 threadId, int64 id) 
		:_dbInterface(pdbi),
		 _threadId(threadId),
		 _id(id)
	{}

	virtual void query(const char* tableName, const DBQueryCommandFunc& f, const DBCallBack& cb);
	virtual void insert(const char* tableName, const DBInsertCommandFunc& f, const DBCallBack& cb);
	virtual void update(const char* tableName, const DBUpdateCommandFunc& f, const DBCallBack& cb);
	virtual void del(const char* tableName, const DBDeleteCommandFunc& f, const DBCallBack& cb);
	virtual void save(const char* tableName, const DBSaveCommandFunc& f, const DBCallBack& cb);
	virtual void execRawSql(const int32 optType, const char* sql, const DBCallBack& cb);


	virtual bool onSuccess(sl::api::IKernel* pKernel, const int32 optType, const int32 affectedRow, IMysqlResult* result);
	virtual bool onFailed(sl::api::IKernel* pKernel, const int32 optType, const int32 errCode);
	virtual void onRelease();

	virtual void release() = 0;

protected:
	IDBInterface*	_dbInterface;
	DBCallBack		_cb;
	int64			_threadId;
	int64			_id;
};

template<int32 maxSize>
class DBContext : public DBCall{
public:
	DBContext(IDBInterface* pdbi, int64 threadId, int64 id)
		:DBCall(pdbi, threadId, id),
		_size(0)
	{}

	static IDBCall* create(IDBInterface* pdbi, int64 threadId, int64 id){
		return CREATE_FROM_POOL(s_pool, pdbi, threadId, id);
	}

	void setContext(const void* context, const int32 size){
		SLASSERT(size <= maxSize, "out of range");
		_size = size;
		sl::SafeMemcpy(_context, maxSize, context, size);
	}

	const void* getContext(const int32 size) const {
		SLASSERT(size <= maxSize && size == _size, "wtf");
		return _context;
	}

	void release(){ s_pool.recover(this); }

private:
	char _context[maxSize];
	int32 _size;
	static sl::SLPool<DBContext> s_pool;
};
template<int32 maxSize> sl::SLPool<DBContext<maxSize>> DBContext<maxSize>::s_pool;

#endif
