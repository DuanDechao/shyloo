#include "ObjectMethod.h"
#include "MMObject.h"
const int8 ObjectMethod::getType(IObject* object) const{
	int32 objTypeId = ((MMObject*)object)->getObjectPropInfo()->getObjTypeId();
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");
	
	return _layouts[objTypeId - 1]->_type;
}

const int32 ObjectMethod::getSetting(IObject* object) const {
	int32 objTypeId = ((MMObject*)object)->getObjectPropInfo()->getObjTypeId();
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");

	return _layouts[objTypeId - 1]->_setting;
}

const int32 ObjectMethod::getIndex(IObject* object) const{
	int32 objTypeId = ((MMObject*)object)->getObjectPropInfo()->getObjTypeId();
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");

	return _layouts[objTypeId - 1]->_index;
}

std::vector<uint8> ObjectMethod::getArgs(IObject* object) const{
	int32 objTypeId = ((MMObject*)object)->getObjectPropInfo()->getObjTypeId();
	SLASSERT(objTypeId > 0 && objTypeId <= _size && _layouts[objTypeId - 1], "wtf");
	
	
	return _layouts[objTypeId - 1]->getArgsType();
}