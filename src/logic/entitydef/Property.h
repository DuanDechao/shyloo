#ifndef SL_LOGIC_ENTITY_DEF_PROPERTY_H
#define SL_LOGIC_ENTITY_DEF_PROPERTY_H
#include "slmulti_sys.h"
#include <string>

typedef uint16 ENTITY_PROPERTY_UID;
typedef uint8 DETAIL_TYPE;
class DataType;
class PropertyDescription{
public:
	PropertyDescription(ENTITY_PROPERTY_UID utype, const char* dataTypeName, const char* name, uint32 flags, bool isPersistent, DataType* dataType,
		bool isIdentifier, const char* indexType, uint32 databaseLength, const char* defaultValStr, DETAIL_TYPE detailLevel);
	virtual ~PropertyDescription();

	static PropertyDescription* createDescription(ENTITY_PROPERTY_UID utype, const char* dataTypeName, const char* name, uint32 flags, bool isPersistent, DataType* dataType,
		bool isIdentifier, const char* indexType, uint32 databaseLength, const char* defaultValStr, DETAIL_TYPE detailLevel);

private:
	std::string									_name;							//这个属性的名称
	std::string									_dataTypeName;					//这个属性字符串数据类别名
	uint32										_flags;							//这个属性的传输标志
	bool										_isPersistent;					//是否存储到数据库
	DataType*									_dataType;						//
};
#endif
