#ifndef __SL_PUBLIC_BINARY_MAP_H__
#define __SL_PUBLIC_BINARY_MAP_H__
#include "slmulti_sys.h"
#include "sltools.h"
#include <unordered_map>

namespace sl{
class OBMap{
	typedef std::unordered_map<int32, int32> IndexMap;
	template<typename T> struct Trait {};

public:
	OBMap(const void* context, const int32 size){
		int32 upSize = *(int32*)context;
		_separate = (const char*)context + sizeof(int32)+upSize;
		parseArray(upSize);
	}

	void parseArray(int32 size){
		int32 offset = 0;
		while (offset < size){
			SLASSERT(offset + sizeof(int32) <= size, "wtf");
			if (offset + sizeof(int32) > size)
				break;

			offset += sizeof(int32);
			int32 key = *(int32*)(_separate - offset);
			
			SLASSERT(offset + sizeof(int32) <= size, "wtf");
			if (offset + sizeof(int32) > size)
				break;

			offset += sizeof(int32);
			int32 dataOffset = *(int32*)(_separate - offset);

			_indexs[key] = dataOffset;
		}
	}

	int8 getInt8(const int32 key) { return getData(key, Trait<int8>()); }
	int16 getInt16(const int32 key) { return getData(key, Trait<int16>()); }
	int32 getInt32(const int32 key) { return getData(key, Trait<int32>()); }
	int64 getInt64(const int32 key) { return getData(key, Trait<int64>()); }
	float getFloat(const int32 key) { return getData(key, Trait<float>()); }
	const char* getString(const int32 key) {
		const void* p = getData(key);
		if (p)
			return (const char*)p;
		return "";
	}

	const void* getBlob(const int32 key, int32& size){
		const void* p = getData(key);
		if (p){
			size = *(int32*)p;
			return (const char*)p + sizeof(int32);
		}
		return nullptr;
	}

private:
	template<typename T>
	T getData(const int32 key, const Trait<T>&){
		const void * p = getData(key);
		if (p)
			return *(T*)p;
		return T();
	}

	const void* getData(const int32 key){
		auto itr = _indexs.find(key);
		if (itr != _indexs.end()){
			return _separate + itr->second;
		}
		return nullptr;
	}

private:
	const char* _separate;
	IndexMap	_indexs;
};




template<int32 upSize, int32 downSize>
class IBMap{
public:
	IBMap() : _fixed(false), _upOffset(0), _downOffset(0), _context(nullptr), _size(0){
		_separate = _data + sizeof(int32) + upSize;
	}

	IBMap& writeInt8(const int32 key, int8 val) { writeData(key, val); return *this; }
	IBMap& writeInt16(const int32 key, int16 val) { writeData(key, val); return *this; }
	IBMap& writeInt32(const int32 key, int32 val) { writeData(key, val); return *this; }
	IBMap& writeInt64(const int32 key, int64 val) { writeData(key, val); return *this; }
	IBMap& writeFloat(const int32 key, float val) { writeData(key, val); return *this; }
	IBMap& writeString(const int32 key, const char* val) {
		SLASSERT(!_fixed, "has fixed");
		int32 len = (int32)strlen(val);
		SLASSERT(_downOffset + len + 1 <= downSize, "out of range");
		if (_downOffset + len + 1 <= downSize){
			sl::SafeMemcpy(_separate + _downOffset, downSize - _downOffset, val, len);
			_separate[_downOffset + len] = 0;
			writeDesc(key, _downOffset);
			_downOffset += len + 1;
		}
		return *this;
	}

	IBMap& writeBlob(const int32 key, const void* val, const int32 size){
		SLASSERT(!_fixed, "has fixed");
		SLASSERT(_downOffset + size + sizeof(int32) <= downSize, "out of range");
		if (_downOffset + size + sizeof(int32) <= downSize){
			writeDesc(key, _downOffset);
			sl::SafeMemcpy(_separate + _downOffset, downSize - _downOffset, &size, sizeof(int32));
			_downOffset += sizeof(int32);
			sl::SafeMemcpy(_separate + _downOffset, downSize - _downOffset, val, size);
			_downOffset += size;
		}
		return *this;
	}

	inline void fix(){
		SLASSERT(!_fixed, "has fixed");

		_fixed = true;
		int32& reserve = *(int32*)(_data + upSize + sizeof(int32)-_upOffset - sizeof(int32));
		reserve = _upOffset;

		_size = sizeof(int32)+_upOffset + _downOffset;
		_context = &reserve;
	}

	inline const void* getContext() const{
		SLASSERT(_fixed, "not fixed");
		return _context;
	}

	inline int32 getSize() const{
		SLASSERT(_fixed, "not fixed");
		return _size;
	}

private:
	void writeDesc(const int32 key, int32 offset){
		SLASSERT(!_fixed, "has fixed");
		SLASSERT(_upOffset + sizeof(int32) + sizeof(int32) <= upSize, "out of range");
		if (_upOffset + sizeof(int32)+sizeof(int32) <= upSize){
			_upOffset += sizeof(int32);
			sl::SafeMemcpy(_separate - _upOffset, sizeof(int32), &key, sizeof(int32));

			_upOffset += sizeof(int32);
			sl::SafeMemcpy(_separate - _upOffset, sizeof(int32), &offset, sizeof(int32));
		}
	}

	template<typename T>
	void writeData(const int32 key, T& val){
		SLASSERT(!_fixed, "has fixed");
		SLASSERT(_downOffset + sizeof(T) <= downSize, "out of range");
		if (_downOffset + sizeof(T) <= downSize){
			sl::SafeMemcpy(_separate + _downOffset, downSize - _downOffset, &val, sizeof(T));
			writeDesc(key, _downOffset);
			_downOffset += sizeof(T);
		}
	}
private:
	char	_data[upSize + downSize + sizeof(int32)];
	int32	_upOffset;
	int32	_downOffset;
	char*	_separate;

	bool	_fixed;
	void*	_context;
	int32	_size;
};
}

#endif