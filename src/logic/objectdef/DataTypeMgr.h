#ifndef SL_LOGIC_ENTITY_DEF_DATA_TYPE_MGR_H
#define SL_LOGIC_ENTITY_DEF_DATA_TYPE_MGR_H
#include "slmulti_sys.h"
#include <unordered_map>
#include "IDCCenter.h"
#include "IObjectDef.h"
#include "slxml_reader.h"
class DataTypeMgr{
public:
	typedef std::unordered_map<std::string, uint16> TYPE_MAP;
	typedef std::unordered_map<std::string, IDataType*> DATATYPE_MAP;
	typedef std::unordered_map<uint32, uint16> UID_TYPE_MAP;
	typedef std::unordered_map<uint32, IDataType*> UID_DATATYPE_MAP;
	DataTypeMgr();
	virtual ~DataTypeMgr();

	static bool initialize(const char* aliasFile);
	static uint16 getType(const char* name);
	static uint16 getType(const uint32 uid);
	static IDataType* getDataType(const sl::ISLXmlNode& typeNode);
	static IDataType* getDataType(const char* name);
	static IDataType* getDataType(const uint32 uid);
	static void addDataType(const char* name, IDataType* dataType);
	static const std::unordered_map<std::string, IDataType*>& getAllDataType(){return _dataTypes;}
	static bool loadAlias(const char* file);

private:
	static TYPE_MAP _typeMap;
	static DATATYPE_MAP _dataTypes;
	static UID_TYPE_MAP _uidTypeMap;
	static UID_DATATYPE_MAP _uidDataTypeMap;
	static uint32 _dataTypeUID; 
};
#endif
