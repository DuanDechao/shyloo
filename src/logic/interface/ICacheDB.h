#ifndef __SL_INTERFACE_ICACHE_H__
#define __SL_INTERFACE_ICACHE_H__
#include "slimodule.h"

class ICacheDBReader{
public:
	virtual ~ICacheDBReader() {}
	virtual void readColumn(const char* column) = 0;
};

typedef std::function<void(sl::api::IKernel* pKernel, ICacheDBReader* reader)> CacheDBColumnFuncType;

class ICacheDBReadResult{
public:
	virtual ~ICacheDBReadResult() {}
	
	virtual int32 count() = 0;

	virtual int8 getInt8(int32 row, int32 col) const = 0;
	virtual int16 getInt16(int32 row, int32 col) const = 0;
	virtual int32 getInt32(int32 row, int32 col) const = 0;
	virtual int64 getInt64(int32 row, int32 col) const = 0;
	virtual float getFloat(int32 row, int32 col) const = 0;
	virtual const char* getString(int32 row, int32 col) const = 0;
	virtual const void* getBinary(int32 row, int32 col, int32& size) const = 0;
};

typedef std::function<void(sl::api::IKernel* pKernel, ICacheDBReadResult* result)> CacheDBReadFuncType;

class ICacheDBContext{
public:
	virtual ~ICacheDBContext() {}

	virtual void writeInt8(const char* col, int8 val) = 0;
	virtual void writeInt16(const char* col, int16 val) = 0;
	virtual void writeInt32(const char* col, int32 val) = 0;
	virtual void writeInt64(const char* col, int64 val) = 0;
	virtual void writeFloat(const char* col, float val) = 0;
	virtual void writeString(const char* col, const char* val) = 0;
	virtual void writeStruct(const char* col, const void* val, const int32 size) = 0;
	virtual void writeBlob(const char* col, const void* val, const int32 size) = 0;
};

typedef std::function<void(sl::api::IKernel* pKernel, ICacheDBContext* context)> CacheDBWriteFuncType;

class ICacheDB : public sl::api::IModule{
public:
	virtual ~ICacheDB() {}

	virtual bool read(const char* table, const CacheDBColumnFuncType& cf, const CacheDBReadFuncType& f, int32 count, ...) = 0;
	virtual bool readByIndex(const char* table, const CacheDBColumnFuncType& cf, const CacheDBReadFuncType& f, const int64 index) = 0;
	virtual bool readByIndex(const char* table, const CacheDBColumnFuncType& cf, const CacheDBReadFuncType& f, const char* index) = 0;

	//sync 是否立即同步到数据库
	virtual bool write(const char* table, bool sync, const CacheDBWriteFuncType& f, int32 count, ...) = 0;
	virtual bool writeByIndex(const char* table, bool sync, const CacheDBWriteFuncType& f, const int64 index) = 0;
	virtual bool writeByIndex(const char* table, bool sync, const CacheDBWriteFuncType& f, const char* index) = 0;

	virtual bool del(const char* table, bool sync, int32 count, ...) = 0;
	virtual bool delByIndex(const char* table, bool sync, const int64 index) = 0;
	virtual bool delByIndex(const char* table, bool sync, const char* index) = 0;
};

#endif