#include "DataDict.h"
#include "DataArray.h"
bool DataDict::setData(const char* name, const int8 type, const void* data, const int32 size){
	auto itor = _dictLayouts.find(name);
	if(itor == _dictLayouts.end()){
		SLASSERT(false, "set dict value of %s failed, %s is not in fixedDict", name, name);
		return false;
	}
	
	PropLayout* layout = itor->second;
	if (layout != nullptr){
		if (layout->_type == type && layout->_size >= size){
			_memory->setData(layout, data, size);
			//propCall(prop, sync);
			return true;
		}
	}
	return false;
}

const void* DataDict::getData(const char* name, const int8 type, int32& size) const{
	auto itor = _dictLayouts.find(name);
	if(itor == _dictLayouts.end()){
		SLASSERT(false, "get dict value of %s failed, %s is not in fixedDict", name, name);
		return nullptr;
	}
	
	PropLayout* layout = itor->second;
	if (layout != nullptr){
		if ((layout->_type == type || type == -1) && layout->_size >= size){
			if(layout->_type == DTYPE_DICT){
				DataDict* dict = NEW DataDict(_object, static_cast<DictLayout*>(layout)->_dictEles, _memory->getData(layout), layout->_size);
				return dict;
			}
			else if(layout->_type == DTYPE_ARRAY){
				int32 arrayMemIndex = *(int32*)_memory->getData(layout);
				if(arrayMemIndex == 0){
					arrayMemIndex = const_cast<MMObject*>(_object)->addArrayMemory(static_cast<ArrayLayout*>(layout)->_arrayProp);
					_memory->setData(layout, &arrayMemIndex, sizeof(int32));
				}
				OMemory* memory = const_cast<MMObject*>(_object)->getArrayMemory(arrayMemIndex);
				DataArray* array = NEW DataArray(_object, static_cast<ArrayLayout*>(layout)->_arrayProp, memory);
				return array;
			}
			else{
				size = layout->_size;
				return _memory->getData(layout);
			}
		}
	}
	return nullptr;
}
