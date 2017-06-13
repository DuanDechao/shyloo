#ifndef SL_SLREDIS_H
#define SL_SLREDIS_H
#include "sltype.h"
#include <functional>
namespace sl
{
namespace db
{
class ISLRedisResult{
public:
	virtual int8 SLAPI toInt8() const = 0;
	virtual uint8 SLAPI toUint8() const = 0;
	virtual int16 SLAPI toInt16() const = 0;
	virtual uint16 SLAPI toUint16() const = 0;
	virtual int32 SLAPI toInt32() const = 0;
	virtual uint32 SLAPI toUint32() const = 0;
	virtual int64 SLAPI toInt64() const = 0;
	virtual uint64 SLAPI toUint64() const = 0;
	virtual float SLAPI toFloat() const = 0;
	virtual const char* SLAPI toString() const = 0;
	virtual const void* SLAPI toStruct(const int32 size) const = 0;
	virtual const void* SLAPI toBlob(int32& size) const = 0;
	virtual int32 SLAPI Count() const = 0;
	virtual bool SLAPI GetResult(int32 idx, const std::function<bool(const ISLRedisResult *)>& f) const = 0;
};

class ISLRedisConnection{
public:
	virtual bool SLAPI reconnect() = 0;
	virtual void SLAPI release() = 0;
	virtual bool SLAPI isActive() = 0;
	virtual const char* SLAPI getIp() = 0;
	virtual int32 SLAPI getPort() = 0;
	virtual bool SLAPI exec(char* command, const int32 size, const std::function<bool(ISLRedisResult* result)>& f) = 0;
};

class ISLRedisMgr{
public:
	virtual ISLRedisConnection* SLAPI create(const char* ip, const int32 port, const char* passwd, const int32 timeout) = 0;
	virtual void SLAPI release(void) = 0;
};

extern "C" SL_DLL_API ISLRedisMgr* SLAPI getSLRedisMgr(void);

}
}

#endif