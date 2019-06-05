#include "DataArray.h"
#include "DataDict.h"
bool DataArray::setData(const int32 index, const int8 type, const void* data, const int32 size){
	if(index > length()){
		SLASSERT(false, "invaild index %d, size %d", index, length());
		return false;
	}

	if (_layout != nullptr){
		if (_layout->_type == type && _layout->_size >= size){
			if(index == length()){
				extend(index, 1);
			}
			_memory->setData(sizeof(int32) + index * _layout->_size, _layout, data, size);
		//	propCall(prop, sync);
			return true;
		}
	}
	return false;
}

bool DataArray::repeat(const int32 n){
	if(n <= 0)
		return clear();
	
	if(n == 1)
		return true;

	int32 oldLength = length();
	extend(oldLength, (n-1) * oldLength);

	char* src = _memory->buf() + sizeof(int32);
	for(int32 i = 1; i < n; i++){
		char* dst = _memory->buf() + sizeof(int32) + (oldLength * i) * _layout->_size;
		sl::SafeMemcpy(dst, oldLength * _layout->_size, src, oldLength * _layout->_size);
	}

	return true;
}

//range [start, end)
bool DataArray::remove(const int32 start, const int32 end){
	if(length() <= 0)
		return true;

	if(start < 0 || end > length() || start > end){
		SLASSERT(false, "invaild index start(%d):end(%d) must in [0, %d)", start, end, length() -1);
		return false;
	}
	if(end == length()){
		setLength(start);
		return true;
	}

	char* dst = _memory->buf() + sizeof(int32) + _layout->_size * start;
	char* src = _memory->buf() + sizeof(int32) + _layout->_size * end;

	sl::SafeMemcpy(dst, _memory->size() - start * _layout->_size - sizeof(int32), src, (length() - end) * _layout->_size);
	setLength(length() - (end - start));
	return true;
}

bool DataArray::extend(const int32 index, const int32 count){
	int32 newSize = _memory->size() + count * _layout->_size;
	_memory->resize(newSize);

	if(index == length()){
		setLength(length() + count);
		return true;
	}

	char* dst = _memory->buf() + sizeof(int32) + (index + count) * _layout->_size;
	char* src = _memory->buf() + sizeof(int32) + _layout->_size * index;
	sl::SafeMemcpy(dst, _memory->size() - (index + count) * _layout->_size - sizeof(int32), src, (length() - index) * _layout->_size);
	setLength(length() + count);
	return true;
}

const void* DataArray::getData(const int32 index, const int8 type, int32& size) const{
	if (_layout != nullptr){
		if ((_layout->_type == type || type == -1) && _layout->_size >= size){
			if(_layout->_type == DTYPE_DICT){
				DataDict* dict = NEW DataDict(_object, static_cast<const DictLayout*>(_layout)->_dictEles, 
						_memory->getData(sizeof(int32) + index * _layout->_size, _layout), _layout->_size);
				return dict;
			}
			else if(_layout->_type == DTYPE_ARRAY){
				int32 arrayMemIndex = *(int32*)_memory->getData(_layout);
				if(arrayMemIndex == 0){
					arrayMemIndex = const_cast<MMObject*>(_object)->addArrayMemory(static_cast<const ArrayLayout*>(_layout)->_arrayProp);
					_memory->setData(_layout, &arrayMemIndex, sizeof(int32));
				}
				OMemory* memory = const_cast<MMObject*>(_object)->getArrayMemory(arrayMemIndex);
				DataArray* array = NEW DataArray(_object, static_cast<const ArrayLayout*>(_layout)->_arrayProp, memory);
				return array;
			}
			else{
				return _memory->getData(sizeof(int32) + index * _layout->_size, _layout);	
			}
		}
	}
	return nullptr;
}
