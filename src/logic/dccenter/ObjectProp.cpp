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