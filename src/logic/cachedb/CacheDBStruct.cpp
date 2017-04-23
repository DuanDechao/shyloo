#include "CacheDBStruct.h"

int8 CacheDBReadResult::getInt8(int32 row, int32 col) const{
	int8 value = 0;
	_rst->GetResult(row, [&value, col](const sl::db::ISLRedisResult* data){
		return data->GetResult(col, [&value](const sl::db::ISLRedisResult* result){
			value = result->toInt8();
			return true;
		});
	});
	return value;
}

int16 CacheDBReadResult::getInt16(int32 row, int32 col) const{
	int16 value = 0;
	_rst->GetResult(row, [&value, col](const sl::db::ISLRedisResult* data){
		return data->GetResult(col, [&value](const sl::db::ISLRedisResult* result){
			value = result->toInt16();
			return true;
		});
	});
	return value;
}

int32 CacheDBReadResult::getInt32(int32 row, int32 col) const{
	int32 value = 0;
	_rst->GetResult(row, [&value, col](const sl::db::ISLRedisResult* data){
		return data->GetResult(col, [&value](const sl::db::ISLRedisResult* result){
			value = result->toInt32();
			return true;
		});
	});
	return value;
}

int64 CacheDBReadResult::getInt64(int32 row, int32 col) const{
	int64 value = 0;
	_rst->GetResult(row, [&value, col](const sl::db::ISLRedisResult* data){
		return data->GetResult(col, [&value](const sl::db::ISLRedisResult* result){
			value = result->toInt64();
			return true;
		});
	});
	return value;
}

float CacheDBReadResult::getFloat(int32 row, int32 col) const{
	float value = 0.f;
	_rst->GetResult(row, [&value, col](const sl::db::ISLRedisResult* data){
		return data->GetResult(col, [&value](const sl::db::ISLRedisResult* result){
			value = result->toFloat();
			return true;
		});
	});
	return value;
}

const char* CacheDBReadResult::getString(int32 row, int32 col) const{
	const char* value = "";
	_rst->GetResult(row, [&value, col](const sl::db::ISLRedisResult* data){
		return data->GetResult(col, [&value](const sl::db::ISLRedisResult* result){
			value = result->toString();
			return true;
		});
	});
	return value;
}

const void* CacheDBReadResult::getBinary(int32 row, int32 col, int32& size) const{
	const void* value = nullptr;
	_rst->GetResult(row, [&value, &size, col](const sl::db::ISLRedisResult* data){
		return data->GetResult(col, [&value, &size](const sl::db::ISLRedisResult* result){
			value = result->toBlob(size);
			return true;
		});
	});
	return value;
}