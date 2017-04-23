#ifndef __SL_CORE_CACHEDB_H__
#define __SL_CORE_CACHEDB_H__
#include "ICacheDB.h"
#include "slsingleton.h"
#include <unordered_map>

class IRedis;
class CacheDB : public ICacheDB, public sl::SLHolder<CacheDB>{
	enum{
		CDB_TYPE_NONE = 0,
		CDB_TYPE_INT8,
		CDB_TYPE_INT16,
		CDB_TYPE_INT32,
		CDB_TYPE_INT64,
		CDB_TYPE_STRING,

		CDB_TYPE_CANT_BE_KEY,
		CDB_TYPE_FLOAT = CDB_TYPE_CANT_BE_KEY,
		CDB_TYPE_STRUCT,
		CDB_TYPE_BLOB
	};

public:
	struct CacheTable{
		std::string name;
		std::unordered_map<std::string, int8> columns;
		std::string key;
		bool del;
	};
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual bool read(const char* table, const CacheDBColumnFuncType& cf, const CacheDBReadFuncType& f, int32 count, ...);
	virtual bool readByIndex(const char* table, const CacheDBColumnFuncType& cf, const CacheDBReadFuncType& f, const int64 index);
	virtual bool readByIndex(const char* table, const CacheDBColumnFuncType& cf, const CacheDBReadFuncType& f, const char* index);

	virtual bool write(const char* table, const CacheDBWriteFuncType& f, int32 count, ...);
	virtual bool writeByIndex(const char* table, const CacheDBWriteFuncType& f, const int64 index);
	virtual bool writeByIndex(const char* table, const CacheDBWriteFuncType& f, const char* index);

	virtual bool del(const char* table, int32 count, ...);
	virtual bool delByIndex(const char* table, const int64 index);
	virtual bool delByIndex(const char* table, const char* index);

	void test();

private:
	sl::api::IKernel*		_kernel;
	IRedis*					_redis;

	std::unordered_map<std::string, CacheTable> _tables;
};
#endif