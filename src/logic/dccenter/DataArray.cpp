#include "DataArray.h"
#include "DataDict.h"
bool DataArray::setData(const int32 index, const int8 type, const void* data, const int32 size){
	if (_layout != nullptr){
		if (_layout->_type == type && _layout->_size >= size){
			_memory->setData(sizeof(int32) + index * _layout->_size, _layout, data, size);
		//	propCall(prop, sync);
			return true;
		}
	}
	return false;
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
