#include "slredis_callresult.h"
#include "slstring_utils.h"
namespace sl{
namespace db{
int8 SLRedisCallResult::toInt8() const{
	if (_reply->type == REDIS_REPLY_INTEGER)
		return (int8)_reply->integer;

	if (_reply->type == REDIS_REPLY_STRING)
		return sl::CStringUtils::StringAsInt8(_reply->str);

	if (_reply->type == REDIS_REPLY_NIL)
		return 0;

	SLASSERT(false, "invalid type");
	return 0;
}
uint8 SLRedisCallResult::toUint8() const{
	if (_reply->type == REDIS_REPLY_INTEGER)
		return (uint8)_reply->integer;

	if (_reply->type == REDIS_REPLY_STRING)
		return (uint8)sl::CStringUtils::StringAsInt8(_reply->str);

	if (_reply->type == REDIS_REPLY_NIL)
		return 0;

	SLASSERT(false, "invalid type");
	return 0;
}
int16 SLRedisCallResult::toInt16() const{
	if (_reply->type == REDIS_REPLY_INTEGER)
		return (int16)_reply->integer;

	if (_reply->type == REDIS_REPLY_STRING)
		return sl::CStringUtils::StringAsInt16(_reply->str);

	if (_reply->type == REDIS_REPLY_NIL)
		return 0;

	SLASSERT(false, "invalid type");
	return 0;
}

uint16 SLRedisCallResult::toUint16() const{
	if (_reply->type == REDIS_REPLY_INTEGER)
		return (uint16)_reply->integer;

	if (_reply->type == REDIS_REPLY_STRING)
		return (uint16)sl::CStringUtils::StringAsInt16(_reply->str);

	if (_reply->type == REDIS_REPLY_NIL)
		return 0;

	SLASSERT(false, "invalid type");
	return 0;
}
int32 SLRedisCallResult::toInt32() const{
	if (_reply->type == REDIS_REPLY_INTEGER)
		return (int32)_reply->integer;

	if (_reply->type == REDIS_REPLY_STRING)
		return sl::CStringUtils::StringAsInt32(_reply->str);

	if (_reply->type == REDIS_REPLY_NIL)
		return 0;

	SLASSERT(false, "invalid type");
	return 0;
}
uint32 SLRedisCallResult::toUint32() const{
	if (_reply->type == REDIS_REPLY_INTEGER)
		return (uint32)_reply->integer;

	if (_reply->type == REDIS_REPLY_STRING)
		return (uint32)sl::CStringUtils::StringAsInt32(_reply->str);

	if (_reply->type == REDIS_REPLY_NIL)
		return 0;

	SLASSERT(false, "invalid type");
	return 0;
}
int64 SLRedisCallResult::toInt64() const{
	if (_reply->type == REDIS_REPLY_INTEGER)
		return (int64)_reply->integer;

	if (_reply->type == REDIS_REPLY_STRING)
		return sl::CStringUtils::StringAsInt64(_reply->str);

	if (_reply->type == REDIS_REPLY_NIL)
		return 0;

	SLASSERT(false, "invalid type");
	return 0;
}
uint64 SLRedisCallResult::toUint64() const{
	if (_reply->type == REDIS_REPLY_INTEGER)
		return (uint64)_reply->integer;

	if (_reply->type == REDIS_REPLY_STRING)
		return (uint64)sl::CStringUtils::StringAsInt64(_reply->str);

	if (_reply->type == REDIS_REPLY_NIL)
		return 0;

	SLASSERT(false, "invalid type");
	return 0;
}
float SLRedisCallResult::toFloat() const{
	if (_reply->type == REDIS_REPLY_STRING)
		return sl::CStringUtils::StringAsFloat(_reply->str);

	if (_reply->type == REDIS_REPLY_NIL)
		return 0.0;

	SLASSERT(false, "invalid type");
	return 0.0;
}
const char* SLRedisCallResult::toString() const{
	if (_reply->type == REDIS_REPLY_STRING)
		return _reply->str;

	if (_reply->type == REDIS_REPLY_NIL)
		return "";

	SLASSERT(false, "invalid type");
	return "";
}

const void* SLRedisCallResult::toStruct(const int32 size) const {
	if (_reply->type == REDIS_REPLY_STRING) {
		SLASSERT(_reply->len == size, "wtf");
		return _reply->str;
	}

	if (_reply->type == REDIS_REPLY_NIL)
		return nullptr;

	SLASSERT(false, "invalid type");
	return nullptr;
}

const void* SLRedisCallResult::toBlob(int32& size) const {
	if (_reply->type == REDIS_REPLY_STRING) {
		size = _reply->len;
		return _reply->str;
	}

	if (_reply->type == REDIS_REPLY_NIL)
		return nullptr;

	SLASSERT(false, "invalid type");
	return nullptr;
}

int32 SLRedisCallResult::Count() const {
	if (_reply->type == REDIS_REPLY_ARRAY) {
		return (int32)_reply->elements;
	}

	SLASSERT(false, "invalid type");
	return 0;
}

bool SLRedisCallResult::GetResult(int32 idx, const std::function<bool(const ISLRedisResult *)>& f) const {
	if (_reply->type == REDIS_REPLY_ARRAY) {
		SLASSERT(idx >= 0 && idx < _reply->elements, "out of range");
		if (idx >= 0 && (uint32)idx < _reply->elements) {
			SLRedisCallResult rst(_reply->element[idx]);
			return f(&rst);
		}
	}

	SLASSERT(false, "invalid type");
	return false;
}

}
}
