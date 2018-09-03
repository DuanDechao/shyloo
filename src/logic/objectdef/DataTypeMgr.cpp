#include "DataTypeMgr.h"
#include <algorithm>
#include "IPythonServer.h"
DataTypeMgr::TYPE_MAP DataTypeMgr::_typeMap;
DataTypeMgr::DATATYPE_MAP DataTypeMgr::_dataTypes;
DataTypeMgr::UID_TYPE_MAP DataTypeMgr::_uidTypeMap;
DataTypeMgr::UID_DATATYPE_MAP DataTypeMgr::_uidDataTypeMap;
uint32 DataTypeMgr::_dataTypeUID = 0;
bool DataTypeMgr::initialize(const char* aliasFile){
	//初始化一些基本类别
	_typeMap["UINT8"] = DTYPE_UINT8;
	_typeMap["UINT16"] = DTYPE_UINT16;
	_typeMap["UINT32"] = DTYPE_UINT32;
	_typeMap["UINT64"] = DTYPE_UINT64;
	_typeMap["INT8"] = DTYPE_INT8;
	_typeMap["INT16"] = DTYPE_INT16;
	_typeMap["INT32"] = DTYPE_INT32;
	_typeMap["INT64"] = DTYPE_INT64;
	_typeMap["FLOAT"] = DTYPE_FLOAT;
	_typeMap["STRING"] = DTYPE_STRING;
	_typeMap["BLOB"] = DTYPE_BLOB;
	
	if(!loadAlias(aliasFile))
		return false;

	return true;
}

bool DataTypeMgr::loadAlias(const char* file){
	sl::XmlReader conf;
	if (!conf.loadXml(file)){
		SLASSERT(false, "can not load file %s", file);
		return false;
	}
	
	const std::vector<sl::ISLXmlNode*>& types = conf.root().getAllChilds();
	for (auto type : types){
		string aliasName = type->value();
		string value = type->text();
		IDataType* dataType = DataTypeMgr::getDataType(value.c_str());
		if(NULL == dataType){
			dataType = SLMODULE(PythonServer)->createPyDataTypeFromXml(value.c_str(), type);
		}
		SLASSERT(dataType, "load dataType failed[%s]", aliasName.c_str());
		addDataType(aliasName.c_str(), dataType);
	}

	return true;
}

uint16 DataTypeMgr::getType(const char* name){
	auto itor = _typeMap.find(name);
	if (itor != _typeMap.end())
		return itor->second;
	return -1;
}

uint16 DataTypeMgr::getType(const uint32 uid){
	auto itor = _uidTypeMap.find(uid);
	if(itor != _uidTypeMap.end())
		return itor->second;
	return -1;
}

IDataType* DataTypeMgr::getDataType(const sl::ISLXmlNode& typeNode){
	const char* name = typeNode.text();
	IDataType* dataType = getDataType(name);
	if(!dataType)
		return SLMODULE(PythonServer)->createPyDataTypeFromXml(name, &typeNode);
	return dataType;
}
IDataType* DataTypeMgr::getDataType(const char* name){
	auto itor = _dataTypes.find(name);
	if(itor != _dataTypes.end())
		return itor->second;

	IDataType* dataType = SLMODULE(PythonServer)->createPyDataType(name);
	if(dataType)
		addDataType(name, dataType);
	return dataType;
}

IDataType* DataTypeMgr::getDataType(const uint32 uid){
	auto itor = _uidDataTypeMap.find(uid);
	if(itor != _uidDataTypeMap.end())
		return itor->second;
	
	return nullptr;
}

void DataTypeMgr::addDataType(const char* name, IDataType* dataType){
	_dataTypeUID++;
	dataType->setAliasName(name);
	if(_typeMap.find(name) == _typeMap.end())
		_typeMap[name] = DTYPE_BLOB;

	_dataTypes[name] = dataType;
	_uidTypeMap[_dataTypeUID] = _typeMap[name];
	_uidDataTypeMap[_dataTypeUID] = dataType;
	dataType->setUid(_dataTypeUID);
}

