#ifndef SL_ARGS_H
#define SL_ARGS_H
#include "slmulti_sys.h"
#include "sltools.h"

enum {
	ARGS_TYPE_UNKNOWN = 0,
	ARGS_TYPE_BOOL,
	ARGS_TYPE_INT8,
	ARGS_TYPE_INT16,
	ARGS_TYPE_INT32,
	ARGS_TYPE_INT64,
	ARGS_TYPE_FLOAT,
	ARGS_TYPE_STRING,
	ARGS_TYPE_STRUCT,
};
#pragma pack(push, 1)
struct arg_info{
	int8 type;
	int16 offset;
};

template<int32 maxCount, int32 maxSize>
struct args_header{
	int32 reserve;
	arg_info argsInfo[maxCount];
	int32 argsCount;
	int16 dataOffset;
	char data[maxSize];
};
#pragma pack(pop)

class OArgs{
public:
	OArgs(const void* pContext, const int32 size) : _context(pContext), _size(size){
		_count = (int32*)_context;
		if (*_count == 0){
			SLASSERT(_size == (sizeof(int32)+sizeof(int32)+sizeof(int16)), "invlaild args num");
		}
		else{
			SLASSERT(_size > (int32)(sizeof(int32)+sizeof(arg_info)*(*_count) + sizeof(int32)+sizeof(int16)), "invlaild args num");
		}
		_args = (arg_info *)(((const char*)pContext) + sizeof(int32));
		_data = (const char*)((char*)pContext + sizeof(int32)+sizeof(arg_info)* (*_count) + sizeof(int32)+sizeof(int16));
		_dataSize = _size - (sizeof(int32)+sizeof(arg_info)*(*_count) + sizeof(int32)+sizeof(int16));
	}

	inline int8 getType(const int32 index)const {
		return getArgs(index).type;
	}

	inline int32 getCount() const { return *_count; }

	inline const void* getContext() const { return _context; }

	inline int32 getSize() const { return _size; }

	bool getBool(const int32 index) const{
		const arg_info& info = getArgs(index);
		SLASSERT(info.type == ARGS_TYPE_BOOL && info.offset + (int16)sizeof(bool) <= _dataSize, "out of range");
		return *(bool*)(_data + info.offset);
	}

	int8 getInt8(const int32 index) const{
		const arg_info& info = getArgs(index);
		SLASSERT(info.type == ARGS_TYPE_INT8 && info.offset + (int16)sizeof(int8) <= _dataSize, "out of range");
		return *(int8*)(_data + info.offset);
	}

	int16 getInt16(const int32 index) const{
		const arg_info& info = getArgs(index);
		SLASSERT(info.type == ARGS_TYPE_INT16 && info.offset + (int16)sizeof(int16) <= _dataSize, "out of range");
		return *(int16*)(_data + info.offset);
	}

	int32 getInt32(const int32 index) const{
		const arg_info& info = getArgs(index);
		SLASSERT(info.type == ARGS_TYPE_INT32 && info.offset + (int16)sizeof(int32) <= _dataSize, "out of range");
		return *(int32*)(_data + info.offset);
	}

	int64 getInt64(const int32 index) const{
		const arg_info& info = getArgs(index);
		SLASSERT(info.type == ARGS_TYPE_INT64 && info.offset + (int16)sizeof(int64) <= _dataSize, "out of range");
		return *(int64*)(_data + info.offset);
	}

	float getFloat(const int32 index) const{
		const arg_info& info = getArgs(index);
		SLASSERT(info.type == ARGS_TYPE_FLOAT && info.offset + (int16)sizeof(float) <= _dataSize, "out of range");
		return *(float*)(_data + info.offset);
	}

	const char* getString(const int32 index) const{
		const arg_info& info = getArgs(index);
		SLASSERT(info.type == ARGS_TYPE_STRING, "out of range");
		return _data + info.offset;
	}

	const void* getStruct(const int32 index, int32& size) const{
		const arg_info& info = getArgs(index);
		SLASSERT(info.type == ARGS_TYPE_STRUCT && info.offset + sizeof(int32) <= _dataSize, "out of range");
		size = *(int32*)(_data + info.offset);
		return _data + info.offset + sizeof(int32);
	}

private:
	const arg_info& getArgs(const int32 index) const{
		SLASSERT(index < *_count, "out of range");
		return _args[*_count - 1 - index];
	}
private:
	const void* _context;
	const int32 _size;

	int32 * _count;
	arg_info * _args;
	const char* _data;
	int32 _dataSize;
};


template<int32 maxCount, int32 maxSize>
class IArgs{
public:
	IArgs() :_bFixed(false),_pContext(nullptr), _size(0){
		_header.reserve = 0;
		_header.argsCount = 0;
		_header.dataOffset = 0;
	}

	IArgs& operator << (const bool value){
		return write(ARGS_TYPE_BOOL, (const char*)&value, sizeof(value));
	}

	IArgs& operator << (const int8 value){
		return write(ARGS_TYPE_INT8, (const char*)&value, sizeof(value));
	}

	IArgs& operator << (const int16 value){
		return write(ARGS_TYPE_INT16, (const char*)&value, sizeof(value));
	}

	IArgs& operator << (const int32 value){
		return write(ARGS_TYPE_INT32, (const char*)&value, sizeof(value));
	}

	IArgs& operator << (const int64 value){
		return write(ARGS_TYPE_INT64, (const char*)&value, sizeof(value));
	}

	IArgs& operator << (const uint64 value){
		return write(ARGS_TYPE_INT64, (const char*)&value, sizeof(value));
	}

	IArgs& operator << (const float value){
		return write(ARGS_TYPE_FLOAT, (const char*)&value, sizeof(value));
	}

	IArgs& operator << (const char* string){
		int32 size = (int32)strlen(string) + 1;
		if (_bFixed || _header.argsCount >= maxCount || _header.dataOffset + size > maxSize){
			SLASSERT(false, "can not contain more args");
			return *this;
		}
		arg_info& info = _header.argsInfo[maxCount - 1 - _header.argsCount];
		info.type = ARGS_TYPE_STRING;
		info.offset = _header.dataOffset;
		SafeSprintf(_header.data + _header.dataOffset, size, "%s", string);
		_header.argsCount++;
		_header.dataOffset += size;
		return *this;
	}

	IArgs& addStruct(const void* context, const int32 size){
		if (_bFixed || _header.argsCount >= maxCount || _header.dataOffset + size + sizeof(int32) > maxSize){
			SLASSERT(false, "can not contain more args");
			return *this;
		}
		arg_info& info = _header.argsInfo[maxCount - 1 - _header.argsCount];
		info.type = ARGS_TYPE_STRUCT;
		info.offset = _header.dataOffset;

		sl::SafeMemcpy(_header.data + _header.dataOffset, maxSize - _header.dataOffset, &size, sizeof(int32));
		_header.dataOffset += sizeof(int32);
		sl::SafeMemcpy(_header.data + _header.dataOffset, maxSize - _header.dataOffset, context, size);
		_header.dataOffset += size;
		_header.argsCount++;

		return *this;
	}

	IArgs& operator <<(const OArgs& args){
		for (int32 i = 0; i < args.getCount(); i++){
			switch (args.getType(i)){
			case ARGS_TYPE_INT8: *this << args.getInt8(i); break;
			case ARGS_TYPE_INT16: *this << args.getInt16(i); break;
			case ARGS_TYPE_INT32: *this << args.getInt32(i); break;
			case ARGS_TYPE_INT64: *this << args.getInt64(i); break;
			case ARGS_TYPE_FLOAT: *this << args.getFloat(i); break;
			case ARGS_TYPE_STRING: *this << args.getString(i); break;
			case ARGS_TYPE_BOOL: *this << args.getBool(i); break;
			case ARGS_TYPE_STRUCT: {int32 size = 0; (*this).addStruct(args.getStruct(i, size), size); break; }
			default: SLASSERT(false, "unknown type");  break;
			}
		}
		return *this;
	}

	IArgs& append(const OArgs& args, int32 count){
		for (int32 i = 0; i < count; i++){
			switch (args.getType(i)){
			case ARGS_TYPE_INT8: *this << args.getInt8(i); break;
			case ARGS_TYPE_INT16: *this << args.getInt16(i); break;
			case ARGS_TYPE_INT32: *this << args.getInt32(i); break;
			case ARGS_TYPE_INT64: *this << args.getInt64(i); break;
			case ARGS_TYPE_FLOAT: *this << args.getFloat(i); break;
			case ARGS_TYPE_STRING: *this << args.getString(i); break;
			case ARGS_TYPE_BOOL: *this << args.getBool(i); break;
			case ARGS_TYPE_STRUCT: {int32 size = 0; (*this).addStruct(args.getStruct(i, size), size); break; }
			default: SLASSERT(false, "unknown type");  break;
			}
		}
		return *this;
	}

	inline bool* reserveBoolean() { return (bool*)reserveData(ARGS_TYPE_BOOL, sizeof(bool)); }
	inline int8* reserveInt8() { return (int8*)reserveData(ARGS_TYPE_INT8, sizeof(int8)); }
	inline int16* reserveInt16() { return (int8*)reserveData(ARGS_TYPE_INT16, sizeof(int16)); }
	inline int32* reserveInt32() { return (int32*)reserveData(ARGS_TYPE_INT32, sizeof(int32)); }
	inline int64* reserveInt64() { return (int64*)reserveData(ARGS_TYPE_INT64, sizeof(int64)); }
	inline float* reserveFloat() { return (float*)reserveData(ARGS_TYPE_FLOAT, sizeof(float)); }

	inline OArgs out(){
		if (!_bFixed){
			SLASSERT(false, "must fixed");
			fix();
		}
		if (_size < 0){
			SLASSERT(false, "wtf");
		}
		return OArgs(_pContext, _size);
	}

	inline void fix(){
		SLASSERT(!_bFixed, "wtf");
		_bFixed = true;
		//int32 len = sizeof(_header);
		int32& reserve = *(int32*)((const char*)(_header.argsInfo + maxCount - _header.argsCount) - sizeof(int32));
		reserve = _header.argsCount;

		_size = sizeof(int32)+sizeof(arg_info)*_header.argsCount + sizeof(int32)+sizeof(int16)+sizeof(int8)*_header.dataOffset;
		_pContext = &reserve;
	}

	inline const void* getContext() const{ return _pContext; }
	inline int32 getSize() const { return _size; }

private:
	IArgs& write(int8 type, const char* pContext, const int32 size){
		if (_bFixed || _header.argsCount >= maxCount || _header.dataOffset + size > maxSize){
			SLASSERT(false, "can not contain more args");
			return *this;
		}
		arg_info& info = _header.argsInfo[maxCount - 1 - _header.argsCount];
		info.type = type;
		info.offset = _header.dataOffset;
		sl::SafeMemcpy(_header.data + _header.dataOffset, maxSize - _header.dataOffset, pContext, size);
		_header.argsCount++;
		_header.dataOffset += size;
		
		return *this;
	}

	void* reserveData(int8 type, const int32 size){
		if (_bFixed || _header.argsCount >= maxCount || _header.dataOffset + size > maxSize){
			SLASSERT(false, "can not contain more args");
			return *this;
		}

		arg_info& info = _header.argsInfo[maxCount - 1 - _header.argsCount];
		info.type = type;
		info.offset = _header.dataOffset;
		void* data = _header.data + _header.dataOffset;

		_header.argsCount++;
		_header.dataOffset += size;

		return data;
	}

private:
	args_header<maxCount, maxSize> _header;
	bool _bFixed;
	void* _pContext;
	int32 _size;

};
#endif
