#include "PyDataType.h"
#include "pyscript/vector2.h"
#include "pyscript/vector3.h"
#include "pyscript/vector4.h"
#include "pyscript/py_memorystream.h"
#include "DataTypeMgr.h"
#include "IPythonEngine.h"
#include "slxml_reader.h"
#include "IIdMgr.h"
#include "FixedDict.h"
#include "FixedArray.h"
bool PyDataType::initialize(const char* file){
	DataTypeMgr::addDataType("UINT8", NEW IntType<uint8>);
	DataTypeMgr::addDataType("UINT16", NEW IntType<uint16>);
	DataTypeMgr::addDataType("UINT32", NEW UInt32Type);
	DataTypeMgr::addDataType("UINT64", NEW UInt64Type);
	
	DataTypeMgr::addDataType("INT8", NEW IntType<int8>);
	DataTypeMgr::addDataType("INT16", NEW IntType<int16>);
	DataTypeMgr::addDataType("INT32", NEW IntType<int32>);
	DataTypeMgr::addDataType("INT64", NEW Int64Type);
	
	DataTypeMgr::addDataType("FLOAT", NEW FloatType);
	DataTypeMgr::addDataType("DOUBLE", NEW DoubleType);
	DataTypeMgr::addDataType("VECTOR2", NEW Vector2Type);
	DataTypeMgr::addDataType("VECTOR3", NEW Vector3Type);
	DataTypeMgr::addDataType("VECTOR4", NEW Vector4Type);
	
	DataTypeMgr::addDataType("STRING", NEW StringType);
	DataTypeMgr::addDataType("UNICODE", NEW UnicodeType);
	DataTypeMgr::addDataType("PYTHON", NEW PythonType);
	DataTypeMgr::addDataType("PY_DICT", NEW PyDictType);
	DataTypeMgr::addDataType("PY_TUPLE", NEW PyTupleType);
	DataTypeMgr::addDataType("PY_LIST", NEW PyListType);
	DataTypeMgr::addDataType("BLOB", NEW BlobType);

	FixedArray::installScript(SLMODULE(PythonEngine)->getPythonModule());
	FixedDict::installScript(SLMODULE(PythonEngine)->getPythonModule());

	if(!loadAlias(file))
		return false;
	
	return true;
}

bool PyDataType::loadAlias(const char* file){
	sl::XmlReader conf;
	if (!conf.loadXml(file)){
		SLASSERT(false, "can not load file %s", file);
		return false;
	}
	
	const std::vector<sl::ISLXmlNode*>& types = conf.root().getAllChilds();
	for (auto type : types){
		string aliasName = type->value();
		string value = type->text();

		if(value == "FIXED_DICT"){
			PyFixedDictType* fixedDict = NEW PyFixedDictType();
			if(!fixedDict->initType(type)){
				printf("PyDataType::loadAlias: parase FIXED_DICT(%s) error!\n", aliasName.c_str());
				DEL fixedDict;
				return false;
			}

			DataTypeMgr::addDataType(aliasName.c_str(), fixedDict);

		}else if(value == "ARRAY"){
			PyFixedArrayType* fixedArray = NEW PyFixedArrayType();
			if(!fixedArray->initType(type)){
				printf("PyDataType::loadAlias: parase FIXED_ARRAY(%s) error!\n", aliasName.c_str());
				DEL fixedArray;
				return false;
			}

			DataTypeMgr::addDataType(aliasName.c_str(), fixedArray);
		}else{
			IDataType* dataType = DataTypeMgr::getDataType(value.c_str());
			if(!dataType){
				printf("PyDataType::loadAlias: cant found type(%s) by alias(%s)\n", value.c_str(), aliasName.c_str());
				return false;
			}

			DataTypeMgr::addDataType(aliasName.c_str(), dataType);
		}
	}

	return true;
}

void* PyDataType::createFromObject(IObject* object, const IProp* prop){
	int32 size = 0;
	const void* data = object->getPropData(prop, size);
	const sl::OBStream stream((const char*)data, size);
	return createFromStream(stream);
}

void PyDataType::addDataTypeInfo(sl::IBStream& stream){
	stream << getUid();                                                                                                                                                                                                                                     
	stream << getName();
	stream << getAliasName();
}

IDataType* PyDataType::createDataType(const sl::ISLXmlNode& typeNode){
	std::string typeName = typeNode.text();
	if(typeName == "ARRAY"){
		PyFixedArrayType *fixedArray = NEW PyFixedArrayType();
		fixedArray->initType(&typeNode);
		return fixedArray;
	}
	return nullptr;
}

void UInt64Type::addToStream(sl::IBStream& stream, void* value){
	PyObject* pyValue = (PyObject*)value;
	uint64 v = static_cast<uint64>(PyLong_AsUnsignedLongLong(pyValue));
	if(PyErr_Occurred()){
		PyErr_Clear();
		v = (uint64)PyLong_AsUnsignedLong(pyValue);
		if(PyErr_Occurred()){
			PyErr_Clear();
			v = (uint64)PyLong_AsLong(pyValue);

			if(PyErr_Occurred()){
				PyErr_Clear();
				PyErr_Format(PyExc_TypeError, "UInt64Type::addToStream: pyValue(%s) is wrong!",
						(pyValue == NULL) ? "NULL": pyValue->ob_type->tp_name);

				PyErr_PrintEx(0);

				v = 0;
			}
		}
	}
	stream << v;
}

void UInt64Type::addToObject(IObject* object, const IProp* prop, void* value){
	int32 size = 0;
	const char* data = (const char*)(object->getPropData(prop, size));
	sl::IBStream stream(const_cast<char*>(data), size);
	addToStream(stream, value);
}

void* UInt64Type::createFromStream(const sl::OBStream& stream){
	uint64 val = 0;
	if(!stream.readUint64(val)){
		ECHO_ERROR("read UINT64 from stream failed");
		return nullptr;
	}

	PyObject* pyVal = PyLong_FromUnsignedLongLong(val);
	if(PyErr_Occurred()){
		PyErr_Format(PyExc_TypeError, "UInt64Type::createFromStream: errval=%lld", val);
		PyErr_PrintEx(0);
		//SL_RELEASE(pyVal);
		if(pyVal){
			Py_DECREF(pyVal);
			pyVal = NULL;
		}
		return PyLong_FromUnsignedLongLong(0);
	}

	return pyVal;
}

void* UInt64Type::parseDefaultStr(const char* defaultVal){
	uint64 val = sl::CStringUtils::StringAsUint64(defaultVal);
	sl::OBStream stream((char*)&val, sizeof(uint64));
	return createFromStream(stream);
}

void UInt32Type::addToStream(sl::IBStream& stream, void* value){
	PyObject* pyValue = (PyObject*)value;
	uint32 v = PyLong_AsUnsignedLong(pyValue);
	if(PyErr_Occurred()){
		PyErr_Clear();
		v = (uint32)PyLong_AsLong(pyValue);

		if(PyErr_Occurred()){
			PyErr_Print();
			PyErr_Clear();

			PyErr_Format(PyExc_TypeError, "UInt32Type::pyValueToUint32: pyValue(%s) is wrong!", 
					(pyValue == NULL) ? "NULL" : pyValue->ob_type->tp_name);
			PyErr_PrintEx(0);

			v = 0;
		}
	}
	stream << v;
}

void UInt32Type::addToObject(IObject* object, const IProp* prop, void* value){
	int32 size = 0;
	const char* data = (const char*)(object->getPropData(prop, size));
	sl::IBStream stream(const_cast<char*>(data), size);
	addToStream(stream, value);
}

void* UInt32Type::createFromStream(const sl::OBStream& stream){
	uint32 val = 0;
	if(!stream.readUint32(val)){
		ECHO_ERROR("read UINT32 from stream failed");
		return nullptr;
	}

	PyObject* pyVal = PyLong_FromUnsignedLong(val);
	if(PyErr_Occurred()){
		PyErr_Format(PyExc_TypeError, "UInt32Type::createFromStream: errval=%u", val);
		PyErr_PrintEx(0);
		//SL_RELEASE(pyVal);
		if(pyVal){
			Py_DECREF(pyVal);
			pyVal = NULL;
		}
		return PyLong_FromUnsignedLong(0);
	}
	return pyVal;
}

void* UInt32Type::parseDefaultStr(const char* defaultValStr){
	uint32 val = (uint32)sl::CStringUtils::StringAsInt64(defaultValStr);
	sl::OBStream stream((char*)&val, sizeof(uint32));
	return createFromStream(stream);
}

void Int64Type::addToStream(sl::IBStream& stream, void* value){
	PyObject* pyValue = (PyObject*)value;
	int64 v = PyLong_AsLongLong(pyValue);
	if(PyErr_Occurred()){
		PyErr_Clear();
		v = (uint32)PyLong_AsLong(pyValue);

		if(PyErr_Occurred()){
			PyErr_Clear();

			PyErr_Format(PyExc_TypeError, "Int64Type::pyValueToInt64: pyValue(%s) is wrong!",
					(pyValue == NULL) ? "NULL": pyValue->ob_type->tp_name);
			PyErr_PrintEx(0);

			v = 0;
		}
	}
	stream << v;
}

void Int64Type::addToObject(IObject* object, const IProp* prop, void* value){
	int32 size = 0;
	const char* data = (const char*)(object->getPropData(prop, size));
	sl::IBStream stream(const_cast<char*>(data), size);
	addToStream(stream, value);
}

void* Int64Type::createFromStream(const sl::OBStream& stream){
	int64 val = 0;
	if(!stream.readInt64(val)){
		ECHO_ERROR("read INT64 from stream failed");
		return nullptr;
	}
	PyObject* pyVal = PyLong_FromLongLong(val);
	if(PyErr_Occurred()){
		PyErr_Format(PyExc_TypeError, "Int64Type::createFromStream: errval=%lld", val);
		PyErr_PrintEx(0);
		if(pyVal){
			Py_DECREF(pyVal);
			pyVal = NULL;
		}

		return PyLong_FromLongLong(0);
	}
	return pyVal;
}

void* Int64Type::parseDefaultStr(const char* defaultValStr){
	int64 val = sl::CStringUtils::StringAsInt64(defaultValStr);
	sl::OBStream stream((char*)&val, sizeof(int64));
	return createFromStream(stream);
}

void FloatType::addToStream(sl::IBStream& stream, void* value){
	PyObject* pyValue = (PyObject*)value;
	float val = 0.0;
	if(!PyFloat_Check(pyValue)){
		PyErr_Format(PyExc_TypeError, "FloatType::pyValueToFloat: pyValue(%s) is wrong!",
				(pyValue == NULL) ? "NULL" : pyValue->ob_type->tp_name);
		PyErr_PrintEx(0);
	}
	else{
		val = (float)PyFloat_AsDouble(pyValue);
	}
	stream << val;
}

void FloatType::addToObject(IObject* object, const IProp* prop, void* value){
	int32 size = 0;
	const char* data = (const char*)(object->getPropData(prop, size));
	sl::IBStream stream(const_cast<char*>(data), size);
	addToStream(stream, value);
}

void* FloatType::createFromStream(const sl::OBStream& stream){
	float val = 0;
	if(!stream.readFloat(val)){
		ECHO_ERROR("read FLOAT from stream failed");
		return nullptr;
	}

	PyObject* pyVal = PyFloat_FromDouble(val);
	if(PyErr_Occurred()){
		PyErr_Format(PyExc_TypeError, "FloatType::createFromStream: errorval=%f", val);
		PyErr_PrintEx(0);
		if(pyVal){
			Py_DECREF(pyVal);
			pyVal = NULL;
		}
		return PyFloat_FromDouble(0);
	}
	return pyVal;
}

void* FloatType::parseDefaultStr(const char* defaultStr){
	float val = sl::CStringUtils::StringAsFloat(defaultStr);
	sl::OBStream stream((char*)&val, sizeof(float));
	return createFromStream(stream);
}

void DoubleType::addToStream(sl::IBStream& stream, void* value){
	PyObject* pyValue = (PyObject*)value;
	double val = 0.0;
	if(!PyFloat_Check(pyValue)){
		PyErr_Format(PyExc_TypeError, "DoubleType::pyValueToDouble: pyValue(%s) is wrong!",
				(pyValue == NULL) ? "NULL" : pyValue->ob_type->tp_name);
		PyErr_PrintEx(0);
	}
	else{
		val =  PyFloat_AsDouble(pyValue);
	}

	stream << val;
}

void DoubleType::addToObject(IObject* object, const IProp* prop, void* value){
	int32 size = 0;
	const char* data = (const char*)(object->getPropData(prop, size));
	sl::IBStream stream(const_cast<char*>(data), size);
	addToStream(stream, value);
}

void* DoubleType::createFromStream(const sl::OBStream& stream){
	double val = 0.0;
	if(!stream.readDouble(val)){
		ECHO_ERROR("read DOUBLE from stream failed");
		return nullptr;
	}

	PyObject* pyVal = PyFloat_FromDouble(val);
	if(PyErr_Occurred()){
		PyErr_Format(PyExc_TypeError, "DoubleType::createFromStream: errval=%f", val);
		PyErr_PrintEx(0);
		if(pyVal){
			Py_DECREF(pyVal);
			pyVal = NULL;
		}
		return PyFloat_FromDouble(0);
	}
	return pyVal;
}

void* DoubleType::parseDefaultStr(const char* defaultValStr){
	double val = sl::CStringUtils::StringAsDouble(defaultValStr);
	sl::OBStream stream((char*)&val, sizeof(val));
	return createFromStream(stream);
}

void Vector2Type::addToStream(sl::IBStream& stream, void* value){
	PyObject* pyValue = (PyObject*)value;
	for(int32 index = 0; index < 2; index++){
		PyObject* pyVal = PySequence_GetItem(pyValue, index);
		float v = (float)PyFloat_AsDouble(pyVal);
		stream << v;
		Py_DECREF(pyVal);
	}
}

void Vector2Type::addToObject(IObject* object, const IProp* prop, void* value){
	int32 size = 0;
	const char* data = (const char*)(object->getPropData(prop, size));
	sl::IBStream stream(const_cast<char*>(data), size);
	addToStream(stream, value);
}

void* Vector2Type::createFromStream(const sl::OBStream& stream){
	float x = 0.0, y = 0.0;
	if(!stream.readFloat(x) || !stream.readFloat(y)){
		ECHO_ERROR("read VECTOR2 from stream failed");
		return nullptr;
	}

	return (PyObject*)(NEW sl::pyscript::ScriptVector2(x, y));
}

void* Vector2Type::parseDefaultStr(const char* defaultValStr){
	float x = 0.0f, y = 0.0f;
	if(!defaultValStr && strcmp(defaultValStr, "") != 0){
		std::stringstream sstream;
		sstream << defaultValStr;
		sstream >> x >> y;
	}

	return (PyObject*)(NEW sl::pyscript::ScriptVector2(x, y));
}

void Vector3Type::addToStream(sl::IBStream& stream, void* value){
	PyObject* pyValue = (PyObject*)value;
	for(int32 index = 0; index < 3; index++){
		PyObject* pyVal = PySequence_GetItem(pyValue, index);
		float v = (float)PyFloat_AsDouble(pyVal);
		stream << v;
		Py_DECREF(pyVal);
	}
}

void Vector3Type::addToObject(IObject* object, const IProp* prop, void* value){
	int32 size = 0;
	const char* data = (const char*)(object->getPropData(prop, size));
	sl::IBStream stream(const_cast<char*>(data), size);
	addToStream(stream, value);
}

void* Vector3Type::createFromStream(const sl::OBStream& stream){
	float x = 0.0, y = 0.0, z = 0.0;
	if(!stream.readFloat(x) || !stream.readFloat(y) || !stream.readFloat(z)){
		ECHO_ERROR("read VECTOR3 from stream failed");
		return nullptr;
	}

	return (PyObject*)(NEW sl::pyscript::ScriptVector3(x, y, z));
}

void* Vector3Type::parseDefaultStr(const char* defaultValStr){
	float x = 0.0f, y = 0.0f, z = 0.0f;
	if(!defaultValStr && strcmp(defaultValStr, "") != 0){
		std::stringstream sstream;
		sstream << defaultValStr;
		sstream >> x >> y >> z;
	}

	return (PyObject*)(NEW sl::pyscript::ScriptVector3(x, y, z));
}

void Vector4Type::addToStream(sl::IBStream& stream, void* value){
	PyObject* pyValue = (PyObject*)value;
	for(int32 index = 0; index < 4; index++){
		PyObject* pyVal = PySequence_GetItem(pyValue, index);
		float v = (float)PyFloat_AsDouble(pyVal);
		stream << v;
		Py_DECREF(pyVal);
	}
}

void Vector4Type::addToObject(IObject* object, const IProp* prop, void* value){
	int32 size = 0;
	const char* data = (const char*)(object->getPropData(prop, size));
	sl::IBStream stream(const_cast<char*>(data), size);
	addToStream(stream, value);
}

void* Vector4Type::createFromStream(const sl::OBStream& stream){
	float x = 0.0, y = 0.0, z = 0.0, t = 0.0;
	if(!stream.readFloat(x) || !stream.readFloat(y) || !stream.readFloat(z) || !stream.readFloat(t)){
		ECHO_ERROR("read VECTOR4 from stream failed");
		return nullptr;
	}

	return (PyObject*)(NEW sl::pyscript::ScriptVector4(x, y, z, t));
}

void* Vector4Type::parseDefaultStr(const char* defaultValStr){
	float x= 0.0f, y = 0.0f, z = 0.0f, t = 0.0f;
	if(!defaultValStr && strcmp(defaultValStr, "") != 0){
		std::stringstream sstream;
		sstream << defaultValStr;
		sstream >> x >> y >> z >> t;
	}

	return (PyObject*)(NEW sl::pyscript::ScriptVector4(x, y, z, t));
}

void StringType::addToStream(sl::IBStream& stream, void* value){
	PyObject* pyValue = (PyObject*)value;
	wchar_t* pyUnicode_AsWideCharStringRet0 = PyUnicode_AsWideCharString(pyValue, NULL);
	sl::CStringUtils::wchar2char(pyUnicode_AsWideCharStringRet0, stream);
	PyMem_Free(pyUnicode_AsWideCharStringRet0);
}

void StringType::addToObject(IObject* object, const IProp* prop, void* value){
	int32 size = 0;
	const char* data = (const char*)(object->getPropData(prop, size));
	sl::IBStream stream(const_cast<char*>(data), size);
	addToStream(stream, value);
}

void* StringType::createFromStream(const sl::OBStream& stream){
	const char* data = nullptr;
	if(!stream.readString(data)){
		ECHO_ERROR("read STRING from stream failed");
		return nullptr;
	}

	PyObject* pyObj = PyUnicode_FromString((const char*)data);
	if(pyObj && !PyErr_Occurred())
		return pyObj;

	PyErr_PrintEx(0);
	if(pyObj){
		Py_DECREF(pyObj);
		pyObj = NULL;
	}
	return NULL;
}

void* StringType::parseDefaultStr(const char* defaultValStr){
	PyObject* pyObj = PyUnicode_FromString(defaultValStr);
	if(pyObj && !PyErr_Occurred())
		return pyObj;

	PyErr_PrintEx(0);
	if(pyObj){
		Py_DECREF(pyObj);
		pyObj = NULL;
	}
	return NULL;
}

void UnicodeType::addToStream(sl::IBStream& stream, void* value){
	PyObject* pyObj = PyUnicode_AsUTF8String((PyObject*)value);
	if(pyObj == NULL){
		return;
	}
	stream.addBlob(PyBytes_AS_STRING(pyObj), (int32)PyBytes_GET_SIZE(pyObj));
	Py_DECREF(pyObj);
}

void UnicodeType::addToObject(IObject* object, const IProp* prop, void* value){
	int32 size = 0;
	const char* data = (const char*)(object->getPropData(prop, size));
	sl::IBStream stream(const_cast<char*>(data), size);
	addToStream(stream, value);
}

void* UnicodeType::createFromStream(const sl::OBStream& stream){
	int32 size = 0;
	const void* data = stream.readBlob(size);	
	if(!data){
		ECHO_ERROR("read UNICODE from stream failed");
		return nullptr;
	}

	PyObject* pyObj = PyUnicode_DecodeUTF8((const char*)data, size, "");
	if(pyObj && !PyErr_Occurred()){
		return pyObj;
	}

	if(pyObj){
		Py_DECREF(pyObj);
		pyObj = NULL;
	}
	PyErr_PrintEx(0);

	return NULL;
}

void* UnicodeType::parseDefaultStr(const char* defaultValStr){
	if(!defaultValStr)
		return PyUnicode_DecodeUTF8("", 0, "");
	
	PyObject* pyObj = PyUnicode_DecodeUTF8(defaultValStr, strlen(defaultValStr) + 1, "");
	if(pyObj && !PyErr_Occurred())
		return pyObj;

	PyErr_Clear();
	PyErr_Format(PyExc_TypeError, "UnicodeType::parseDefaultStr: defaultVal(%s) error! val=[%s]", pyObj != NULL ? pyObj->ob_type->tp_name : "NULL", defaultValStr);
	PyErr_PrintEx(0);
	if(pyObj){
		Py_DECREF(pyObj);
		pyObj = NULL;
	}
	return PyUnicode_DecodeUTF8("", 0, "");
}

void BlobType::addToStream(sl::IBStream& stream, void* value){
	PyObject* pyValue = (PyObject*)value;
	if(!PyBytes_Check(pyValue)){
		sl::pyscript::PyMemoryStream* pPyMemoryStream = static_cast<sl::pyscript::PyMemoryStream*>(pyValue);
		sl::MemoryStream& m = pPyMemoryStream->stream();
		stream.addBlob((const char*)m.data() + m.rpos(), (int32)m.length());
	}
	else{
		Py_ssize_t dataSize = PyBytes_GET_SIZE(pyValue);
		char* datas = PyBytes_AsString(pyValue);
		stream.addBlob(datas, (int32)dataSize);
	}
}

void BlobType::addToObject(IObject* object, const IProp* prop, void* value){
	int32 size = 0;
	const char* data = (const char*)(object->getPropData(prop, size));
	sl::IBStream stream(const_cast<char*>(data), size);
	addToStream(stream, value);
}

void* BlobType::createFromStream(const sl::OBStream& stream){
	int32 size = 0;
	const char* data = (const char*)stream.readBlob(size);
	return PyBytes_FromStringAndSize(const_cast<char*>(data), size);
}

void* BlobType::parseDefaultStr(const char* defaultValStr){
	if(defaultValStr)
		return PyBytes_FromStringAndSize(defaultValStr, strlen(defaultValStr) + 1);
	std::string str = "";
	return PyBytes_FromStringAndSize(str.data(), str.size());
}

void PythonType::addToStream(sl::IBStream& stream, void* value){
	PyObject* pyValue = (PyObject*)value;
	std::string datas = SLMODULE(PythonEngine)->pickle(pyValue);
	stream.addBlob(datas.data(), datas.size());
}

void PythonType::addToObject(IObject* object, const IProp* prop, void* value){
	int32 size = 0;
	const char* data = (const char*)(object->getPropData(prop, size));
	sl::IBStream stream(const_cast<char*>(data), size);
	addToStream(stream, value);
}

void* PythonType::createFromStream(const sl::OBStream& stream){
	int32 size = 0;
	const void* data = stream.readBlob(size);
	if(!data){
		ECHO_ERROR("read Python from stream failed");
		return nullptr;
	}

	std::string datas((const char*)data, size);
	return SLMODULE(PythonEngine)->unpickle(datas);
}

void* PythonType::parseDefaultStr(const char* defaultValStr){
	if(!defaultValStr || strcmp(defaultValStr, "") == 0)
		S_Return;

	PyObject* module = PyImport_AddModule("__main__");
	if(module == NULL){
		PyErr_SetString(PyExc_SystemError, "PythonType::createObject::PyImport_AddModule __main__ error");
		PyErr_PrintEx(0);
		S_Return;
	}

	PyObject* mdict = PyModule_GetDict(module);
	return PyRun_String(const_cast<char*>(defaultValStr), Py_eval_input, mdict, mdict);
}

void* PyDictType::parseDefaultStr(const char* defaultValStr){
	PyObject* pyVal = (PyObject*)PythonType::parseDefaultStr(defaultValStr);
	if(PyDict_Check(pyVal))
		return pyVal;

	if(pyVal)
		Py_DECREF(pyVal);

	return PyDict_New();
}

void* PyTupleType::parseDefaultStr(const char* defaultValStr){
	PyObject* pyVal = (PyObject*)PythonType::parseDefaultStr(defaultValStr);
	if(PyTuple_Check(pyVal))
		return pyVal;

	if(pyVal)
		Py_DECREF(pyVal);

	return PyTuple_New(0);
}

void* PyListType::parseDefaultStr(const char* defaultValStr){
	PyObject* pyVal = (PyObject*)PythonType::parseDefaultStr(defaultValStr);
	if(PyList_Check(pyVal))
		return pyVal;

	if(pyVal)
		Py_DECREF(pyVal);

	return PyList_New(0);
}


bool PyFixedArrayType::initType(const sl::ISLXmlNode* typeNode){
	if(!typeNode->subNodeExist("of") || (*typeNode)["of"].count() != 1){
		printf("error!! alias type[%s] has no of!!\n", typeNode->value());
		return false;
	}

	const sl::ISLXmlNode& subTypeNode = (*typeNode)["of"][0];
	std::string strType = subTypeNode.text();
	if(strType == "ARRAY"){
		PyFixedArrayType* dataType = NEW PyFixedArrayType();
		if(!dataType->initType(&subTypeNode)){
			printf("FixedArrayType::initType: Array is wrong!\n");
			return false;
		}

		_dataType = dataType;
		std::string dataName = std::string("_") + sl::CStringUtils::Int64AsString(SLMODULE(IdMgr)->generateLocalId()) + dataType->getName();
		DataTypeMgr::addDataType(dataName.c_str(), dataType);
				
	}else{
		IDataType* dataType = DataTypeMgr::getDataType(strType.c_str());
		if(!dataType){
			printf("FixedArrayType::initType: key[%s] don't find type [%s]\n", "ARRAY", strType.c_str());
			return false;
		}

		_dataType = dataType;
	}

	if(_dataType == NULL){
		printf("FixedArrayType::initType: dataType is NULL\n");
		return false;
	}

	return true;
}

void PyFixedArrayType::addDataTypeInfo(sl::IBStream& stream){
	PyDataType::addDataTypeInfo(stream);
	stream << _dataType->getUid();
}

void PyFixedArrayType::addToObject(IObject* object, const IProp* prop, void* value){
	int32 size = 0;
	const char* data = (const char*)(object->getPropData(prop, size));
	sl::IBStream stream(const_cast<char*>(data), size);
	addToStream(stream, value);
}

void PyFixedArrayType::addToStream(sl::IBStream& stream, void* value){
	uint32 size = (uint32)PySequence_Size((PyObject*)value);
	stream << size;

	for(uint32 i = 0; i < size; i++){
		PyObject* pyVal = PySequence_GetItem((PyObject*)value, i);
		_dataType->addToStream(stream, pyVal);
	}
}

void* PyFixedArrayType::createFromStream(const sl::OBStream& stream){
	uint32 size = 0;
	FixedArray* pFixedArray = NEW FixedArray(this);
	pFixedArray->initialize("");
	
	if(!stream.readUint32(size)){
		ECHO_ERROR("read UINT32 from stream failed");
		return nullptr;
	}

	std::vector<PyObject*>& vals = pFixedArray->getValues();
	for(int32 i = 0; i < size; i++){
		PyObject* pyVal = (PyObject*)(_dataType->createFromStream(stream));
		if(pyVal){
			vals.push_back(pyVal);
		}
		else{
			ECHO_ERROR("FixedArrayType::createFromStream: %s, pyVal is NULL", getAliasName());
			break;
		}
	}

	return (PyObject*)pFixedArray;
	
}

void* PyFixedArrayType::parseDefaultStr(const char* defaultValStr){
	FixedArray* pFixedArray = NEW FixedArray(this);
	pFixedArray->initialize(defaultValStr);
	return (PyObject*)pFixedArray;
}

bool PyFixedDictType::initType(const sl::ISLXmlNode* typeNode){
	if(!typeNode->subNodeExist("Properties")){
		printf("error!! alias type[%s] has no Properties!!\n", typeNode->value());
		return false;
	}

	const sl::ISLXmlNode& propertiesNode = (*typeNode)["Properties"][0];
	const std::vector<sl::ISLXmlNode*>& allPropNode = propertiesNode.getAllChilds();

	std::string typeName = "", strType = "";
	for(auto subType : allPropNode){
		typeName = subType->value();
		if(!subType->subNodeExist("Type") || (*subType)["Type"].count() != 1){
			printf("error! aliasType[%s]'s subType[%s] has no Type attribute!!\n", typeNode->value(), typeName.c_str());
			return false;
		}

		const sl::ISLXmlNode& subTypeNode = (*subType)["Type"][0];
		strType = subTypeNode.text();
		if(strType == "ARRAY"){
			PyFixedArrayType* dataType = NEW PyFixedArrayType();
			if(!dataType->initType(&subTypeNode)){
				printf("PyFixedDictType::initType: key[%s] dont find array-type\n", typeName.c_str(), strType.c_str());
				return false;
			}
			
			_dictItems.push_back(std::pair<std::string, IDataType*>(typeName, dataType));
			std::string dataTypeName = std::string("_") + sl::CStringUtils::Int64AsString(SLMODULE(IdMgr)->generateLocalId()) + dataType->getName();
			DataTypeMgr::addDataType(dataTypeName.c_str(), dataType);
		}
		else{
			IDataType* dataType = DataTypeMgr::getDataType(strType.c_str());
			if(!dataType){
				printf("PyFixedDictType::initType: key[%s] dont find type[%s]\n", typeName.c_str(), strType.c_str());
				return false;
			}

			_dictItems.push_back(std::pair<std::string, IDataType*>(typeName, dataType));
		}

	}

	if(typeNode->subNodeExist("implementedBy")){
		 strType = (*typeNode)["implementedBy"][0].text();
		 if(strType.size() > 0 && !loadImplModule(strType)){
			 printf("PyFixedDictType::initType: loadImplModule is error\n");
			 return false;
		 }
		 _moduleName = strType;
	}

	if(_dictItems.size() == 0){
		printf("FixedDictType::initType: FIXED_DICT(%s) no keys!\n", typeNode->value());
		return false;
	}

	return true;
}

bool PyFixedDictType::loadImplModule(std::string moduleName){
	std::vector<std::string> res;
	sl::CStringUtils::Split(moduleName, ".", res);
	if(res.size() != 2){
		printf("FixedDictType::loadImplModule: %s impl error!\n", moduleName.c_str());
		return false;
	}

	PyObject* implModule = PyImport_ImportModule(const_cast<char*>(res[0].c_str()));
	if(!implModule){
		SCRIPT_ERROR_CHECK();
		return false;
	}

	_pyImplObj = PyObject_GetAttrString(implModule, res[1].c_str());
	Py_DECREF(implModule);

	if(!_pyImplObj){
		SCRIPT_ERROR_CHECK();
		return false;
	}

	_pyCreateObjFromDict = PyObject_GetAttrString(_pyImplObj, "createObjFromDict");
	if(!_pyCreateObjFromDict){
		SCRIPT_ERROR_CHECK();
		return false;
	}

	_pyGetDictFromObj = PyObject_GetAttrString(_pyImplObj, "getDictFromObj");
	if(!_pyGetDictFromObj){
		SCRIPT_ERROR_CHECK();
		return false;
	}

	_pyIsSameType = PyObject_GetAttrString(_pyImplObj, "isSameType");
	if(!_pyIsSameType){
		SCRIPT_ERROR_CHECK();
		return false;
	}

	return true;
}

void PyFixedDictType::addDataTypeInfo(sl::IBStream& stream){
	PyDataType::addDataTypeInfo(stream);
	uint8 itemsize = _dictItems.size(); 
	stream << itemsize;
	stream << _moduleName.c_str();
	for(auto item : _dictItems){
		stream << item.first.c_str();
		stream << item.second->getUid();
	}
}

void PyFixedDictType::addToStream(sl::IBStream& stream, void* value){
	PyObject* pyValue = (PyObject*)value;
	if(hasImpl()){
		pyValue = implGetDictFromObj(pyValue);
	}

	PyObject* pyDict = pyValue;
	if(PyObject_TypeCheck(pyValue, FixedDict::getScriptType())){
		pyDict = static_cast<FixedDict*>(pyValue)->getDictObject();
	}

	auto itor = _dictItems.begin();
	for(; itor != _dictItems.end(); ++itor){
		PyObject* pyObject = PyDict_GetItemString(pyDict, const_cast<char*>(itor->first.c_str()));
		if(pyObject == NULL){
			ECHO_ERROR("PyFixedDictType::addToStream: %s not found key[%s]", getAliasName(), itor->first.c_str());
			
			//itor->second->addToStream(stream, )
			continue;
		}

		itor->second->addToStream(stream, pyObject);
	}

	if(hasImpl()){
		Py_DECREF(pyValue);
	}

}

void* PyFixedDictType::createFromStream(const sl::OBStream& stream){
	FixedDict* pyDict = NEW FixedDict(this, false);
	pyDict->initialize(stream, false);
	if(hasImpl()){
		PyObject* pyValue = implCreateObjFromDict(pyDict);
		Py_DECREF(pyDict);
		return pyValue;
	}

	return (PyObject*)pyDict;
}

PyObject* PyFixedDictType::implCreateObjFromDict(PyObject* dictData){
	PyObject* pyRet = PyObject_CallFunction(_pyCreateObjFromDict, const_cast<char*>("(O)"), dictData);
	if(pyRet == NULL){
		SCRIPT_ERROR_CHECK();
		*((int*)0x0) = 0;
		ECHO_ERROR("fixedDictType::implCreateObjFromDict %s failed", _moduleName.c_str());
		
		Py_RETURN_NONE;
	}

	return pyRet;
}


PyObject* PyFixedDictType::implGetDictFromObj(PyObject* pyObj){
	PyObject* pyRet = PyObject_CallFunction(_pyGetDictFromObj, const_cast<char*>("(O)"), pyObj);
	if(pyRet == NULL){
		SCRIPT_ERROR_CHECK();
		return nullptr;
	}

	return pyRet;
}

void* PyFixedDictType::parseDefaultStr(const char* defaultValStr){
	PyObject* val = PyDict_New();
	FIXEDDICT_KEYTYPE_MAP::iterator iter = _dictItems.begin();
	for(; iter != _dictItems.end(); ++iter){
		PyObject* item = (PyObject*)(iter->second->parseDefaultStr(""));
		PyDict_SetItemString(val, iter->first.c_str(), item);
		Py_DECREF(item);
	}

	FixedDict* pyDict = NEW FixedDict(this);
	pyDict->initialize(val);
	Py_DECREF(val);

	if(hasImpl()){
		PyObject* pyValue = implCreateObjFromDict(pyDict);
		Py_DECREF(pyDict);
		return pyValue;
	}

	return (PyObject*)pyDict;

}
