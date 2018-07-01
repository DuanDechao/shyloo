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
