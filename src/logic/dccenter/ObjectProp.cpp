#include "ObjectProp.h"
#include "MMObject.h"
const int8 ObjectProp::getType(IObject* object) const{
	int32 objTypeId = ((MMObject*)object)->getObjectPropInfo()->getObjTypeId();
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");
	
	return _layouts[objTypeId - 1]->_type;
}

const int32 ObjectProp::getSetting(IObject* object) const {
	int32 objTypeId = ((MMObject*)object)->getObjectPropInfo()->getObjTypeId();
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");

	return _layouts[objTypeId - 1]->_setting;
}

const int32 ObjectProp::getIndex(IObject* object) const{
	int32 objTypeId = ((MMObject*)object)->getObjectPropInfo()->getObjTypeId();
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");

	return _layouts[objTypeId - 1]->_index;
}

const void* ObjectProp::getExtra(IObject* object) const{
	int32 objTypeId = ((MMObject*)object)->getObjectPropInfo()->getObjTypeId();
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");
    
    return _layouts[objTypeId -1]->_extra;
}

const int32 ObjectProp::getSize(IObject* object) const{
	int32 objTypeId = ((MMObject*)object)->getObjectPropInfo()->getObjTypeId();
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");
    
    return _layouts[objTypeId -1]->_size;
}

const char* ObjectProp::getDefaultVal(IObject* object) const{
	int32 objTypeId = ((MMObject*)object)->getObjectPropInfo()->getObjTypeId();
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");
    
    return _layouts[objTypeId -1]->_defaultVal.c_str();
}

const int8 ObjectProp::getType(const int32 objTypeId) const{
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");
	return _layouts[objTypeId - 1]->_type;
}

const int32 ObjectProp::getSetting(const int32 objTypeId) const {
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");
	return _layouts[objTypeId - 1]->_setting;
}

const int32 ObjectProp::getIndex(const int32 objTypeId) const{
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");
	return _layouts[objTypeId - 1]->_index;
}

const void* ObjectProp::getExtra(const int32 objTypeId) const{
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");
    return _layouts[objTypeId -1]->_extra;
}

const int32 ObjectProp::getSize(const int32 objTypeId) const{
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");
    return _layouts[objTypeId -1]->_size;
}

const char* ObjectProp::getDefaultVal(const int32 objTypeId) const{
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");
    return _layouts[objTypeId -1]->_defaultVal.c_str();
}

const int8 ObjectProp::getType(const char* objectType) const{
	int32 objTypeId = SLMODULE(ObjectMgr)->getObjectType(objectType);
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");
	
	return _layouts[objTypeId - 1]->_type;
}

const int32 ObjectProp::getSetting(const char* objectType) const {
	int32 objTypeId = SLMODULE(ObjectMgr)->getObjectType(objectType);
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");

	return _layouts[objTypeId - 1]->_setting;
}

const int32 ObjectProp::getIndex(const char* objectType) const{
	int32 objTypeId = SLMODULE(ObjectMgr)->getObjectType(objectType);
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");

	return _layouts[objTypeId - 1]->_index;
}

const void* ObjectProp::getExtra(const char* objectType) const{
	int32 objTypeId = SLMODULE(ObjectMgr)->getObjectType(objectType);
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");
    
    return _layouts[objTypeId -1]->_extra;
}

const int32 ObjectProp::getSize(const char* objectType) const{
	int32 objTypeId = SLMODULE(ObjectMgr)->getObjectType(objectType);
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");
    
    return _layouts[objTypeId -1]->_size;
}

const char* ObjectProp::getDefaultVal(const char* objectType) const{
	int32 objTypeId = SLMODULE(ObjectMgr)->getObjectType(objectType);
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");
    
    return _layouts[objTypeId -1]->_defaultVal.c_str();
}
	
ISubProp* ObjectProp::addDictProp(const char* objectType, const char* elePropName, const int32 type, const int32 size){
	int32 objTypeId = SLMODULE(ObjectMgr)->getObjectType(objectType);
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");

	return addDictProp(objTypeId, elePropName, type, size);
}

ISubProp* ObjectProp::addDictProp(const int32 objTypeId, const char* elePropName, const int32 type, const int32 size){
	PropLayout* layout = NULL;
	if(type == DTYPE_DICT){
		layout = NEW DictLayout();
	}
	else if(type == DTYPE_ARRAY){
		layout = NEW ArrayLayout();
	}
	else {
		layout = NEW PropLayout();
	}

	layout->_name = elePropName;
	layout->_type = type;
	layout->_size = size;

	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");
	struct DictLayout* dictLayout = static_cast<struct DictLayout*>(_layouts[objTypeId -1]);

	layout->_offset = dictLayout->_innerOffset;
	dictLayout->_innerOffset += size;
	string propKey = elePropName;
	dictLayout->_dictEles[propKey] = layout;

	return NEW ObjectSubProp(layout); 
}

ISubProp* ObjectProp::addArrayProp(const char* objectType, const int32 type, const int32 size){
	int32 objTypeId = SLMODULE(ObjectMgr)->getObjectType(objectType);
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");

	return addArrayProp(objTypeId, type, size);
}

ISubProp* ObjectProp::addArrayProp(const int32 objTypeId, const int32 type, const int32 size){
	PropLayout* layout = NULL;
	if(type == DTYPE_DICT){
		layout = NEW DictLayout();
	}
	else if(type == DTYPE_ARRAY){
		layout = NEW ArrayLayout();
	}
	else {
		layout = NEW PropLayout();
	}

	layout->_type = type;
	layout->_size = size;

	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");
	struct ArrayLayout* arrayLayout = static_cast<struct ArrayLayout*>(_layouts[objTypeId -1]);
	arrayLayout->_arrayProp = layout;

	return NEW ObjectSubProp(layout);
}

ISubProp* ObjectSubProp::addDictProp(const char* elePropName, const int32 type, const int32 size){
	PropLayout* layout = NULL;
	if(type == DTYPE_DICT){
		layout = NEW DictLayout();
	}
	else if(type == DTYPE_ARRAY){
		layout = NEW ArrayLayout();
	}
	else {
		layout = NEW PropLayout();
	}

	layout->_name = elePropName;
	layout->_type = type;
	layout->_size = size;

	
	struct DictLayout* dictLayout = static_cast<struct DictLayout*>(_layout);
	layout->_offset = dictLayout->_innerOffset;
	dictLayout->_innerOffset += size;
	dictLayout->_dictEles[elePropName] = layout;

	return NEW ObjectSubProp(layout);
}

ISubProp* ObjectSubProp::addArrayProp(const int32 type, const int32 size){
	PropLayout* layout = NULL;
	if(type == DTYPE_DICT){
		layout = NEW DictLayout();
	}
	else if(type == DTYPE_ARRAY){
		layout = NEW ArrayLayout();
	}
	else {
		layout = NEW PropLayout();
	}

	layout->_type = type;
	layout->_size = size;
	struct ArrayLayout* arrayLayout = static_cast<struct ArrayLayout*>(_layout);
	arrayLayout->_arrayProp = layout;

	return NEW ObjectSubProp(layout);
}

