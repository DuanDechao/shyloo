#include "ObjectProp.h"
#include "MMObject.h"
const int8 ObjectProp::getType(IObject* object) const{
	int32 objTypeId = ((MMObject*)object)->getObjectPropInfo()->getObjTypeId();
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId], "wtf");
	
	return _layouts[objTypeId]->_type;
}

const int32 ObjectProp::getSetting(IObject* object) const {
	int32 objTypeId = ((MMObject*)object)->getObjectPropInfo()->getObjTypeId();
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId], "wtf");

	return _layouts[objTypeId]->_setting;
}