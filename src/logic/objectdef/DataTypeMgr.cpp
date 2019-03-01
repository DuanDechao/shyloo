#include "DataTypeMgr.h"
#include <algorithm>
#include "IPythonServer.h"
#include "IIdMgr.h"
#include "IDebugHelper.h"
DataTypeMgr::TYPE_MAP DataTypeMgr::_typeMap;
DataTypeMgr::DATATYPE_MAP DataTypeMgr::_dataTypes;
DataTypeMgr::UID_TYPE_MAP DataTypeMgr::_uidTypeMap;
DataTypeMgr::UID_DATATYPE_MAP DataTypeMgr::_uidDataTypeMap;
std::set<IDataType*> DataTypeMgr::_allDataTypes;
uint32 DataTypeMgr::_dataTypeUID = 0;
bool DataTypeMgr::initialize(const char* aliasFile){
	//初始化一些基本类别
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
		string aliasName = type->name();
		string value = type->getValueString();
		IDataType* dataType = DataTypeMgr::getDataType(value.c_str());
		if(NULL == dataType){
			dataType = SLMODULE(PythonServer)->createPyDataTypeFromXml(value.c_str(), type);
		}
		SLASSERT(dataType, "load dataType failed[%s]", aliasName.c_str());
		addDataType(aliasName.c_str(), dataType);
	}

	return true;
}

void DataTypeMgr::release(){
	for(auto typeItor : _allDataTypes){
		DEL typeItor;
	}
	_dataTypes.clear();
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
	const char* name = typeNode.getValueString();
	IDataType* dataType = getDataType(name);
	if(!dataType){
		dataType = SLMODULE(PythonServer)->createPyDataTypeFromXml(name, &typeNode);
		if(dataType){
			std::string dataTypeName = std::string("_") + sl::CStringUtils::Int64AsString(SLMODULE(IdMgr)->generateLocalId()) + name;
			addDataType(dataTypeName.c_str(), dataType);
		}
	}
	return dataType;
}
IDataType* DataTypeMgr::getDataType(const char* name){
	auto itor = _dataTypes.find(name);
	if(itor != _dataTypes.end())
		return itor->second;

	IDataType* dataType = SLMODULE(PythonServer)->createPyDataType(name);
	if(dataType){
		addDataType(name, dataType);
	}
	return dataType;
}

IDataType* DataTypeMgr::getDataType(const uint32 uid){
	auto itor = _uidDataTypeMap.find(uid);
	if(itor != _uidDataTypeMap.end())
		return itor->second;
	
	return nullptr;
}

void DataTypeMgr::addDataType(const char* name, IDataType* dataType){
	if(_typeMap.find(name) != _typeMap.end()){
		ERROR_LOG("duplicate dataType %s", name);
		return;
	}

	_dataTypeUID++;
	dataType->setAliasName(name);
	_typeMap[name] = dataType->getType();

	_dataTypes[name] = dataType;
	_uidTypeMap[_dataTypeUID] = _typeMap[name];
	_uidDataTypeMap[_dataTypeUID] = dataType;
	dataType->setUid(_dataTypeUID);
	_allDataTypes.insert(dataType);
}

