#ifndef __SL_LIB_REDIS_CALLRESULT_H__
#define __SL_LIB_REDIS_CALLRESULT_H__
#include "slredis.h"
#include "hiredis.h"
namespace sl{
namespace db{
class SLRedisCallResult : public ISLRedisResult{
public:
	SLRedisCallResult(redisReply* reply) :_reply(reply){}
	~SLRedisCallResult(){}

	virtual int8 SLAPI toInt8() const;
	virtual uint8 SLAPI toUint8() const;
	virtual int16 SLAPI toInt16() const;
	virtual uint16 SLAPI toUint16() const;
	virtual int32 SLAPI toInt32() const;
	virtual uint32 SLAPI toUint32() const;
	virtual int64 SLAPI toInt64() const;
	virtual uint64 SLAPI toUint64() const;
	virtual float SLAPI toFloat() const;
	virtual const char* SLAPI toString()const;
	virtual const void* SLAPI toStruct(const int32 size) const;
	virtual const void* SLAPI toBlob(int32& size) const;
	virtual int32 SLAPI Count() const;
	virtual bool SLAPI GetResult(int32 idx, const std::function<bool(const ISLRedisResult *)>& f) const;

private:
	redisReply* _reply;
};
}
}


#endif