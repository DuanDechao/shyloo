#ifndef __SL_PUBLIC_BINARY_STREAM_H__
#define __SL_PUBLIC_BINARY_STREAM_H__
#include "slmulti_sys.h"
#include "sltools.h"
namespace sl
{
class OBStream{
public:
	OBStream(const char* buf, const int32 size)
		:_buffer(buf),
		_size(size),
		_offset(0)
	{}
	~OBStream(){}

	inline bool readBoolean(bool& val) const { return read(val); }
	inline bool readInt8(int8& val) const { return read(val); }
	inline bool readInt16(int16& val) const { return read(val); }
	inline bool readInt32(int32& val) const { return read(val); }
	inline bool readInt64(int64& val) const { return read(val); }
	inline bool readFloat(float& val) const { return read(val); }

	bool readString(const char* & val) const {
		int32 size;
		if (!read(size)){
			return false;
		}
		const void* data = getData(size + 1);
		if (data != nullptr){
			val = (const char*)data;
			return true;
		}
		return false;
	}

	inline void reset() { _offset = 0; }

	inline const void * getContext() const { return _buffer; }
	inline const int32 getSize() const { return _size; }

private:
	template<typename T>
	bool read(T& val) const {
		const void* data = getData(sizeof(T));
		if (data == nullptr)
			return false;

		val = *(T*)data;
		return true;
	}

	const void* getData(const int32 size) const{
		SLASSERT(_offset + size < _size, "buffer size over flow");
		if (_offset + size < _size){
			const void* p = _buffer + _offset;
			_offset += size;
			return p;
		}
		return nullptr;
	}

private:
	const char* _buffer;
	int32 _size;
	mutable int32 _offset;
};

template<int32 maxSize>
class IBStream{
public:
	IBStream():_offset(0){}
	~IBStream(){}

	IBStream& operator <<(const bool& val) { return write(val); }
	IBStream& operator <<(const int8& val) { return write(val); }
	IBStream& operator <<(const int16& val) { return write(val); }
	IBStream& operator <<(const int32& val) { return write(val); }
	IBStream& operator <<(const int64& val) { return write(val); }
	IBStream& operator <<(const float& val) { return write(val); }

	IBStream& operator <<(const char* val){
		int32 size = (int32)strlen(val);
		if (_offset + size + sizeof(int32)+1 <= maxSize){
			*this << size;
			sl::SafeMemcpy(_buffer + _offset, maxSize - _offset, val, size);
			_offset += size;
			_buffer[_offset] = 0;
			++_offset;
		}
		return *this;
	}

	inline bool* reserveBoolean() { return (bool*)reserveData(sizeof(bool)); }
	inline int8* reserveInt8() { return (int8*)reserveData(sizeof(int8)); }
	inline int16* reserveInt16() { return (int16*)reserveData(sizeof(int16)); }
	inline int32* reserveInt32() { return (int32*)reserveData(sizeof(int32)); }
	inline int64* reserveInt64() { return (int64*)reserveData(sizeof(int64)); }
	inline float* reserveFloat() { return (float*)reserveData(sizeof(float)); }

	inline OBStream out(){
		return OBStream(_buffer, _offset);
	}

private:
	template<typename T>
	IBStream& write(const T& val){
		if (_offset + sizeof(T) <= maxSize){
			sl::SafeMemcpy(_buffer + _offset, maxSize - _offset, &val, sizeof(T));
			_offset += sizeof(T);
		}
		return *this;
	}

	void* reserveData(int32 size){
		SLASSERT(_offset + size <= maxSize, "wtf");
		if (_offset + size > maxSize)
			return nullptr;

		void* data = _buffer + _offset;
		_offset += size;

		return data;
	}

private:
	char	_buffer[maxSize];
	int32	_offset;
};

}

#endif