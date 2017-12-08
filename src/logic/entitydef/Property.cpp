#include "Property.h"
PropertyDescription::PropertyDescription(ENTITY_PROPERTY_UID utype, const char* dataTypeName, const char* name, uint32 flags, bool isPersistent, DataType* dataType,
	bool isIdentifier, const char* indexType, uint32 databaseLength, const char* defaultValStr, DETAIL_TYPE detailLevel)
{

}

PropertyDescription* PropertyDescription::createDescription(ENTITY_PROPERTY_UID utype, const char* dataTypeName, const char* name, uint32 flags, bool isPersistent, DataType* dataType,
	bool isIdentifier, const char* indexType, uint32 databaseLength, const char* defaultValStr, DETAIL_TYPE detailLevel){
	return NULL;
}