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
	inline bool readUint8(uint8& val) const { return read(val); }
	inline bool readUint16(uint16& val) const { return read(val); }
	inline bool readUint32(uint32& val) const { return read(val); }
	inline bool readUint64(uint64& val) const { return read(val); }
	inline bool readDouble(double& val) const { return read(val); }
	inline const OBStream& operator >> (bool& val) const {read(val); return *this;}
	inline const OBStream& operator >> (int8& val) const {read(val); return *this;}
	inline const OBStream& operator >> (int16& val) const {read(val); return *this;}
	inline const OBStream& operator >> (int32& val) const {read(val); return *this;}
	inline const OBStream& operator >> (int64& val) const {read(val); return *this;}
	inline const OBStream& operator >> (float& val) const {read(val); return *this;}
	inline const OBStream& operator >> (uint8& val) const {read(val); return *this;}
	inline const OBStream& operator >> (uint16& val) const {read(val); return *this;}
	inline const OBStream& operator >> (uint32& val) const {read(val); return *this;}
	inline const OBStream& operator >> (uint64& val) const {read(val); return *this;}
	inline const OBStream& operator >> (double& val) const {read(val); return *this;}
	inline const OBStream& operator >> (const char*& val) const {readString(val); return *this;}

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
		SLASSERT(false, "wtf");
		return false;
	}

	const void* readBlob(int32& size) const {
		if(!read(size)){
			return nullptr;
		}
		
		const void* data = getData(size);
		return data;
	}

	inline void reset() { _offset = 0; }

	inline const void * getContext() const { return _buffer + _offset; }
	inline const int32 getSize() const { return _size - _offset; }

private:
	template<typename T>
	bool read(T& val) const {
		const void* data = getData(sizeof(T));
		if (data == nullptr){
			SLASSERT(false, "wtf");
			return false;
		}

		val = *(T*)data;
		return true;
	}

	const void* getData(const int32 size) const{
		SLASSERT(_offset + size <= _size, "buffer size over flow");
		if (_offset + size <= _size){
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

class IBStream{
public:
	IBStream(char* data, const int32 size):_offset(0),_maxSize(size),_buffer(data){}
	virtual ~IBStream(){}

	virtual IBStream& operator <<(const bool& val) { return write(val); }
	virtual IBStream& operator <<(const int8& val) { return write(val); }
	virtual IBStream& operator <<(const int16& val) { return write(val); }
	virtual IBStream& operator <<(const int32& val) { return write(val); }
	virtual IBStream& operator <<(const int64& val) { return write(val); }
	virtual IBStream& operator <<(const float& val) { return write(val); }
	virtual IBStream& operator <<(const double& val) { return write(val); }

	virtual IBStream& operator <<(const uint8& val) { return write(val); }
	virtual IBStream& operator <<(const uint16& val) { return write(val); }
	virtual IBStream& operator <<(const uint32& val) { return write(val); }
	virtual IBStream& operator <<(const uint64& val) { return write(val); }
	virtual IBStream& operator <<(const sl::OBStream& val) { return writeData(val.getContext(), val.getSize()); }
	
    virtual IBStream& operator <<(const char* val){
		int32 size = (int32)strlen(val);
		if (_offset + size + sizeof(int32)+1 <= _maxSize){
			*this << size;
			sl::SafeMemcpy(_buffer + _offset, _maxSize - _offset, val, size);
			_offset += size;
			_buffer[_offset] = 0;
			++_offset;
		}
		else{
			SLASSERT(false, "wtf");
		}
		return *this;
	}

	virtual IBStream& addBlob(const void* context, const int32 size){
		if(_offset + size + sizeof(int32) <= _maxSize){
			*this << size;
			sl::SafeMemcpy(_buffer + _offset, _maxSize - _offset, context, size);
			_offset += size;
		}
		else{
			SLASSERT(false, "wtf");
		}
		return *this;
	}

	virtual bool* reserveBoolean() { return (bool*)reserveData(sizeof(bool)); }
	virtual int8* reserveInt8() { return (int8*)reserveData(sizeof(int8)); }
	virtual int16* reserveInt16() { return (int16*)reserveData(sizeof(int16)); }
	virtual int32* reserveInt32() { return (int32*)reserveData(sizeof(int32)); }
	virtual int64* reserveInt64() { return (int64*)reserveData(sizeof(int64)); }
	virtual float* reserveFloat() { return (float*)reserveData(sizeof(float)); }
	virtual double* reserveDouble() { return (double*)reserveData(sizeof(double)); }
	virtual char* reserveBuf(const int32 size) { return (char*)reserveData(size); }

	virtual OBStream out(){
		return OBStream(_buffer, _offset);
	}

	char* getBuffer() {return _buffer + _offset;}
	bool skip(const int32 size) {
		if(_offset + size > _maxSize)
			return false;

		_offset += size;
		return true;
	}

private:
	template<typename T>
	IBStream& write(const T& val){
		return writeData(&val, sizeof(T));
	}

	IBStream& writeData(const void* val, const int32 size){
		if (_offset + size <= _maxSize){
			sl::SafeMemcpy(_buffer + _offset, _maxSize - _offset, val, size);
			_offset += size;
		}
		else{
			SLASSERT(false, "wtf");
		}
		return *this;
	}

	void* reserveData(int32 size){
		SLASSERT(_offset + size <= _maxSize, "wtf");
		if (_offset + size > _maxSize)
			return nullptr;

		void* data = _buffer + _offset;
		_offset += size;

		return data;
	}

protected:
	char*   _buffer;
	int32	_offset;
	int32	_maxSize;
};

template<int maxSize>
class BStream: public IBStream{
public:
	BStream():IBStream(_tempBuf, maxSize){}
private:
	char	_tempBuf[maxSize];
};

}

#endif
