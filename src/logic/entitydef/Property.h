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
	std::string									_name;							//������Ե�����
	std::string									_dataTypeName;					//��������ַ������������
	uint32										_flags;							//������ԵĴ����־
	bool										_isPersistent;					//�Ƿ�洢�����ݿ�
	DataType*									_dataType;						//
};
#endif
