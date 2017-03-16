#ifndef __SL_CORE_DB_CALL_H__
#define __SL_CORE_DB_CALL_H__
#include "IDB.h"
#include "IMysqlMgr.h"
#include "sltools.h"

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


	virtual bool onSuccess(sl::api::IKernel* pKernel, const int32 optType, const int32 affectedRow, const MysqlResult& result);
	virtual bool onFailed(sl::api::IKernel* pKernel, const int32 optType, const int32 errCode);
	virtual void onRelease();

	virtual void release() = 0;

protected:
	DB*			_db;
	DBCallBack	_cb;
	int64		_threadId;
	int64		_id;
};

class DBResult : public IDBResult{
public:
	DBResult(const MysqlResult& root) :_root(root){}

	virtual int32 rowCount(){ return (int32)_root.size(); }
	virtual int8 getDataInt8(const int32 i, const char* key){ return sl::CStringUtils::StringAsInt8(getData(i, key).c_str());}
	virtual int16 getDataInt16(const int32 i, const char* key) { return sl::CStringUtils::StringAsInt16(getData(i, key).c_str()); }
	virtual int32 getDataInt32(const int32 i, const char* key){ return sl::CStringUtils::StringAsInt32(getData(i, key).c_str()); }
	virtual int64 getDataInt64(const int32 i, const char* key) { return sl::CStringUtils::StringAsInt64(getData(i, key).c_str()); }
	virtual const char* getDataString(const int32 i, const char* key) { return getData(i, key).c_str(); }

private:
	const std::string& getData(const int32 i, const char* key){
		SLASSERT(i >= 0 && i < (int32)_root.size(), "out of range");
		auto itor = _root[i].find(key);
		SLASSERT(itor != _root[i].end(), "cannot find key %s", key);
		return itor->second;
	}

private:
	const MysqlResult& _root;
};

template<int32 maxSize>
class DBContext : public DBCall{
public:
	DBContext(DB* db, int64 threadId, int64 id)
		:DBCall(db, threadId, id),
		_size(0)
	{}

	void setContext(const void* context, const int32 size){
		SLASSERT(size <= maxSize, "out of range");
		_size = size;
		sl::SafeMemcpy(_context, maxSize, context, size);
	}

	const void* getContext(const int32 size){
		SLASSERT(size <= maxSize && size == _size, "wtf");
		return _context;
	}

	void release(){ DEL this; }

private:
	char _context[maxSize];
	int32 _size;
};

#endif