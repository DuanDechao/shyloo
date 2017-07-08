#ifndef __SL_CACHE_DB_STRUCT_H__
#define __SL_CACHE_DB_STRUCT_H__
#include "ICacheDB.h"
#include "CacheDB.h"
#include "slredis.h"

#define MAX_KEYS 128
#define MAX_ARGS 4096

class CacheDBReader : public ICacheDBReader{
public:
	CacheDBReader(const CacheDB::CacheTable& desc, IArgs<MAX_KEYS, MAX_ARGS>& args) : _desc(desc), _args(args), _count(0){}
	virtual ~CacheDBReader() {}

	virtual void readColumn(const char* column){
		SLASSERT(_desc.columns.find(column) != _desc.columns.end(), "wtf");
		_args << column;
		++_count;
	}

	inline int32 count() const { return _count; }

private:
	const CacheDB::CacheTable& _desc;
	IArgs<MAX_KEYS, MAX_ARGS>& _args;
	int32 _count;
};

class CacheDBReadResult : public ICacheDBReadResult{
public:
	CacheDBReadResult(const sl::db::ISLRedisResult * redisResult) :_rst(redisResult){}
	virtual ~CacheDBReadResult() {}

	virtual int32 count(){ return _rst->Count(); }

	virtual int8 getInt8(int32 row, int32 col) const;
	virtual int16 getInt16(int32 row, int32 col) const;
	virtual int32 getInt32(int32 row, int32 col) const;
	virtual int64 getInt64(int32 row, int32 col) const;
	virtual float getFloat(int32 row, int32 col) const;
	virtual const char* getString(int32 row, int32 col) const;
	virtual const void* getBinary(int32 row, int32 col, int32& size) const;

private:
	const sl::db::ISLRedisResult* _rst;
};

class CacheDBContext : public ICacheDBContext{
public:
	CacheDBContext(const CacheDB::CacheTable& desc, IArgs<MAX_KEYS, MAX_ARGS>& args) : _desc(desc), _args(args),_count(0), _index(false) {}
	virtual ~CacheDBContext() {}

	virtual void writeInt8(const char* col, int8 val) { _args << col << val; ++_count; checkIndex(col); }
	virtual void writeInt16(const char* col, int16 val) { _args << col << val; ++_count; checkIndex(col); }
	virtual void writeInt32(const char* col, int32 val) { _args << col << val; ++_count; checkIndex(col); }
	virtual void writeInt64(const char* col, int64 val) { _args << col << val; ++_count; checkIndex(col); }
	virtual void writeFloat(const char* col, float val) { _args << col << val; ++_count; checkIndex(col); }
	virtual void writeString(const char* col, const char* val) { _args << col << val; ++_count; checkIndex(col); }
	virtual void writeStruct(const char* col, const void* val, const int32 size) { _args << col; _args.addStruct(val, size); ++_count; checkIndex(col); }
	virtual void writeBlob(const char* col, const void* val, const int32 size)  { _args << col;  _args.addStruct(val, size); ++_count; checkIndex(col); }

	inline void checkIndex(const char* col){
		//if (_desc.)
		if (_desc.index.name == col){
			_index = true;
		}
	}

	inline bool isChangedIndex() const { return _index; }
	inline int32 count() const { return _count; }

private:
	const CacheDB::CacheTable&		_desc;
	IArgs<MAX_KEYS, MAX_ARGS>&		_args;
	int32							_count;
	bool							_index;
};

#endif
