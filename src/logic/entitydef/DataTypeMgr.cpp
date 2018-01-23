#include "DataTypeMgr.h"
#include "IDCCenter.h"
DataTypeMgr::DATATYPE_MAP DataTypeMgr::_dataTypes;
DataTypeMgr::UID_DATATYPE_MAP DataTypeMgr::_uid_dataTypes;
DataTypeMgr::TYPE_UID_MAP DataTypeMgr::_typeUidMap;
bool DataTypeMgr::initialize(const char* aliasFile){
	//初始化一些基本类别
	_typeUidMap["UINT8"] = ScriptDataType::SDTYPE_UINT8;
	_typeUidMap["UINT16"] = ScriptDataType::SDTYPE_UINT16;
	_typeUidMap["UINT32"] = ScriptDataType::SDTYPE_UINT32;
	_typeUidMap["UINT64"] = ScriptDataType::SDTYPE_UINT64;
	_typeUidMap["INT8"] = ScriptDataType::SDTYPE_INT8;
	_typeUidMap["INT16"] = ScriptDataType::SDTYPE_INT16;
	_typeUidMap["INT32"] = ScriptDataType::SDTYPE_INT32;
	_typeUidMap["INT64"] = ScriptDataType::SDTYPE_INT64;
	_typeUidMap["STRING"] = ScriptDataType::SDTYPE_STRING;
	_typeUidMap["PYTHON"] = ScriptDataType::SDTYPE_PYTHON;
	_typeUidMap["BLOB"] = ScriptDataType::SDTYPE_BLOB;
	return true;
}

bool DataTypeMgr::addDataType(const char* name, DataType* dataType){
	return true;
}

bool DataTypeMgr::addDataType(DATATYPE_UID uid, DataType* dataType){
	return true;
}

void DataTypeMgr::delDataType(const char* name){

}

//DataType* DataTypeMgr::getDataType(const char* name){
//	auto itor = _dataTypes.find(name);
//	if (itor != _dataTypes.end())
//		return itor->second;
//	
//	SLASSERT(false, "getDataType error: not found type[%s]", name);
//	return NULL;
//}


DATATYPE_UID DataTypeMgr::getDataTypeUid(const char* name){
	auto itor = _typeUidMap.find(name);
	if (itor != _typeUidMap.end())
		return itor->second;

	SLASSERT(false, "getDataType error: not found type[%s]", name);
	return -1;
}

//DataType* DataTypeMgr::getDataType(DATATYPE_UID uid){
//	auto itor = _uid_dataTypes.find(uid);
//	if (itor != _uid_dataTypes.end())
//		return itor->second;
//
//	SLASSERT(false, "getDataType error: not found type[%s]", uid);
//	return NULL;
//}

PyObject* DataTypeMgr::getPyAttrValue(IObject* object, const IProp* prop){
	const int8 dataType = prop->getType(object);
	PyObject* pyValue = nullptr;
	switch (dataType){
	case ScriptDataType::SDTYPE_INT8: pyValue = PyLong_FromLong(object->getPropInt8(prop)); break;
	case ScriptDataType::SDTYPE_INT16:  pyValue = PyLong_FromLong(object->getPropInt16(prop)); break;
	case ScriptDataType::SDTYPE_INT32: pyValue = PyLong_FromLong(object->getPropInt32(prop)); break;
	case ScriptDataType::SDTYPE_INT64: pyValue = PyLong_FromLongLong(object->getPropInt64(prop)); break;
	case ScriptDataType::SDTYPE_UINT8: pyValue = PyLong_FromUnsignedLong((uint8)object->getPropUint8(prop)); break;
	case ScriptDataType::SDTYPE_UINT16:  pyValue = PyLong_FromUnsignedLong((uint16)object->getPropUint16(prop)); break;
	case ScriptDataType::SDTYPE_UINT32: pyValue = PyLong_FromUnsignedLong((uint32)object->getPropUint32(prop)); break;
	case ScriptDataType::SDTYPE_UINT64: pyValue = PyLong_FromUnsignedLongLong((uint64)object->getPropUint64(prop)); break;
	case ScriptDataType::SDTYPE_STRING: pyValue = PyUnicode_FromString(object->getPropString(prop)); break;
	case ScriptDataType::SDTYPE_BLOB: int32 size = 0;  const void* data = object->getPropBlob(prop, size); PyBytes_FromStringAndSize((char*)data, size); break;
	}
	if (PyErr_Occurred()){
		PyErr_Clear();
		PyErr_Format(PyExc_TypeError, "get Attribute Value failed");
		PyErr_PrintEx(0);

		S_RELEASE(pyValue);
		return nullptr;
	}

	return pyValue;
}

int32 DataTypeMgr::setPyAttrValue(IObject* object, const IProp* prop, PyObject* value){
	const int8 dataType = prop->getType(object);
	switch (dataType){
	case ScriptDataType::SDTYPE_INT8: object->setPropInt8(prop,(int8)PyLong_AsLong(value));  break;
	case ScriptDataType::SDTYPE_INT16: object->setPropInt16(prop, (int16)PyLong_AsLong(value)); break;
	case ScriptDataType::SDTYPE_INT32: object->setPropInt32(prop, (int32)PyLong_AsLong(value)); break;
	case ScriptDataType::SDTYPE_INT64: object->setPropInt64(prop, (int64)PyLong_AsLongLong(value)); break;
	case ScriptDataType::SDTYPE_UINT8: object->setPropUint8(prop, (uint8)PyLong_AsUnsignedLong(value)); break;
	case ScriptDataType::SDTYPE_UINT16: object->setPropUint16(prop, (uint16)PyLong_AsUnsignedLong(value)); break;
	case ScriptDataType::SDTYPE_UINT32: object->setPropUint32(prop, (uint32)PyLong_AsUnsignedLong(value)); break;
	case ScriptDataType::SDTYPE_UINT64: object->setPropUint64(prop, (uint64)PyLong_AsUnsignedLongLong(value)); break;
	case ScriptDataType::SDTYPE_STRING: {
		wchar_t*wideCharString = PyUnicode_AsWideCharString(value, NULL);
		char* valStr = sl::CStringUtils::wchar2char(wideCharString);
		PyMem_Free(wideCharString);
		object->setPropString(prop, valStr); 
		break;
	}

	case ScriptDataType::SDTYPE_BLOB:{
		Py_ssize_t datasize = PyBytes_GET_SIZE(value);
		char* datas = PyBytes_AsString(value);
		object->setPropBlob(prop, datas, (uint32)datasize);
		break;
	}
	}
	if (PyErr_Occurred()){
		PyErr_Clear();
		PyErr_Format(PyExc_TypeError, "get Attribute Value failed");
		PyErr_PrintEx(0);

		return -3;
	}
	return 0;
}
