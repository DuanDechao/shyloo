#include "PyDataType.h"
#include "pyscript/vector2.h"
#include "pyscript/vector3.h"
#include "pyscript/vector4.h"
#include "pyscript/py_memorystream.h"
#include "slxml_reader.h"
#include "IIdMgr.h"
#include "FixedDict.h"
#include "FixedArray.h"
#include "IObjectDef.h"
#include "PythonServer.h"
#include "IHarbor.h"
#include "EntityMailBox.h"
#include "IPythonEngine.h"
std::unordered_map<std::string, IDataType*> PyDataType::s_pyDataTypes;
bool PyDataType::initialize(){

	FixedArray::installScript(SLMODULE(PythonEngine)->getBaseModule());
	FixedDict::installScript(SLMODULE(PythonEngine)->getBaseModule());
	return true;
}


void* PyDataType::createFromObject(IObject* object, const IProp* prop){
	int32 size = 0;
	const void* data = object->getPropData(prop, size);
	const sl::OBStream stream((const char*)data, size);
	return createFromStream(stream);
}

void PyDataType::addToObject(IObject* object, const IProp* prop, void* value){
	int32 size = 0;
	const char* data = (const char*)(object->getPropData(prop, size));
	sl::IBStream stream(const_cast<char*>(data), size);
	addToStream(stream, value);
}

void PyDataType::addDataTypeInfo(sl::IBStream& stream){
	stream << getUid();
	stream << getName();
	stream << getAliasName();
}

IDataType* PyDataType::createDataType(const char* typeName, const sl::ISLXmlNode* type){
	if(strcmp(typeName,"FIXED_DICT") == 0){
		PyFixedDictType* fixedDict = NEW PyFixedDictType();
		if(!fixedDict->initType(type)){
			DEL fixedDict;
			return NULL;
		}
		std::string dataTypeName = std::string("_") + sl::CStringUtils::Int64AsString(SLMODULE(IdMgr)->generateLocalId()) + typeName;
		SLMODULE(ObjectDef)->addExtraDataType(dataTypeName.c_str(), fixedDict);
		return fixedDict;

	}else if(strcmp(typeName,"ARRAY") == 0){
		PyFixedArrayType* fixedArray = NEW PyFixedArrayType();
		if(!fixedArray->initType(type)){
			DEL fixedArray;
			return NULL;
		}
		std::string dataTypeName = std::string("_") + sl::CStringUtils::Int64AsString(SLMODULE(IdMgr)->generateLocalId()) + typeName;
		SLMODULE(ObjectDef)->addExtraDataType(dataTypeName.c_str(), fixedArray);
		return fixedArray;

	}

	SLASSERT(false, "invalid data type %s", typeName);
	return NULL;
}

IDataType* PyDataType::createDataType(const char* typeName){
	std::string strTypeName = typeName;
	if(s_pyDataTypes.find(strTypeName) != s_pyDataTypes.end())
		return s_pyDataTypes[strTypeName];

	IDataType* dataType = NULL;
	if(strTypeName == "UINT8"){
		dataType = NEW IntType<uint8>;
	}else if(strTypeName == "UINT16"){
		dataType = NEW IntType<uint16>;
	}else if(strTypeName == "UINT32"){
		dataType = NEW UInt32Type;
	}else if(strTypeName == "UINT64"){
		dataType = NEW UInt64Type;
	}else if(strTypeName == "INT8"){
		dataType = NEW IntType<int8>;
	}else if(strTypeName == "INT16"){
		dataType = NEW IntType<int16>;
	}else if(strTypeName == "INT32"){
		dataType = NEW IntType<int32>;
	}else if(strTypeName == "INT64"){
		dataType = NEW Int64Type;
	}else if(strTypeName == "FLOAT"){
		dataType = NEW FloatType;
	}else if(strTypeName == "DOUBLE"){
		dataType = NEW DoubleType;
	}else if(strTypeName == "VECTOR2"){
		dataType = NEW Vector2Type;
	}else if(strTypeName == "VECTOR3"){
		dataType = NEW Vector3Type;
	}else if(strTypeName == "VECTOR4"){
		dataType = NEW Vector4Type;
	}else if(strTypeName == "STRING"){
		dataType = NEW StringType;
	}else if(strTypeName == "UNICODE"){
		dataType = NEW UnicodeType;
	}else if(strTypeName == "PYTHON"){
		dataType = NEW PythonType;
	}else if(strTypeName == "PY_DICT"){
		dataType = NEW PyDictType;
	}else if(strTypeName == "PY_TUPLE"){
		dataType = NEW PyTupleType;
	}else if(strTypeName == "PY_LIST"){
		dataType = NEW PyListType;
	}else if(strTypeName == "BLOB"){
		dataType = NEW BlobType;
	}else if(strTypeName == "MAILBOX"){
		dataType = NEW MailBoxType; 
	}
	s_pyDataTypes[strTypeName] = dataType;
	return dataType;
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
		printf("error!! alias type[%s] has no of!!\n", typeNode->name());
		return false;
	}

	const sl::ISLXmlNode& subTypeNode = (*typeNode)["of"][0];
	std::string strType = subTypeNode.getValueString();
	if(strType == "ARRAY"){
		PyFixedArrayType* dataType = NEW PyFixedArrayType();
		if(!dataType->initType(&subTypeNode)){
			printf("FixedArrayType::initType: Array is wrong!\n");
			return false;
		}

		_dataType = dataType;
		std::string dataName = std::string("_") + sl::CStringUtils::Int64AsString(SLMODULE(IdMgr)->generateLocalId()) + dataType->getName();
		SLMODULE(ObjectDef)->addExtraDataType(dataName.c_str(), dataType);
				
	}else{
		IDataType* dataType = SLMODULE(ObjectDef)->getDataType(strType.c_str());
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

PyObject* PyFixedArrayType::createNewItemFromObj(PyObject* pyobj){
	return static_cast<PyDataType*>(_dataType)->createNewFromObj(pyobj);
}

PyObject* PyFixedArrayType::createNewFromObj(PyObject* pyobj){
	if(PyObject_TypeCheck(pyobj, FixedArray::getScriptType())){
		Py_INCREF(pyobj);
		return pyobj;
	}
	
	FixedArray* pFixedArray = NEW FixedArray(this);
	pFixedArray->initialize(pyobj);
	return pFixedArray;
}

bool PyFixedDictType::initType(const sl::ISLXmlNode* typeNode){
	if(!typeNode->subNodeExist("Properties")){
		printf("error!! alias type[%s] has no Properties!!\n", typeNode->name());
		return false;
	}

	const sl::ISLXmlNode& propertiesNode = (*typeNode)["Properties"][0];
	const std::vector<sl::ISLXmlNode*>& allPropNode = propertiesNode.getAllChilds();

	std::string typeName = "", strType = "";
	for(auto subType : allPropNode){
		typeName = subType->name();
		if(!subType->subNodeExist("Type") || (*subType)["Type"].count() != 1){
			printf("error! aliasType[%s]'s subType[%s] has no Type attribute!!\n", typeNode->name(), typeName.c_str());
			return false;
		}

		const sl::ISLXmlNode& subTypeNode = (*subType)["Type"][0];
		strType = subTypeNode.getValueString();
		if(strType == "ARRAY"){
			PyFixedArrayType* dataType = NEW PyFixedArrayType();
			if(!dataType->initType(&subTypeNode)){
				printf("PyFixedDictType::initType: key[%s] dont find array-type\n", typeName.c_str(), strType.c_str());
				return false;
			}
			
			_dictItems.push_back(std::pair<std::string, IDataType*>(typeName, dataType));
			std::string dataTypeName = std::string("_") + sl::CStringUtils::Int64AsString(SLMODULE(IdMgr)->generateLocalId()) + dataType->getName();
			SLMODULE(ObjectDef)->addExtraDataType(dataTypeName.c_str(), dataType);
		}
		else{
			IDataType* dataType = SLMODULE(ObjectDef)->getDataType(strType.c_str());
			if(!dataType){
				printf("PyFixedDictType::initType: key[%s] dont find type[%s]\n", typeName.c_str(), strType.c_str());
				return false;
			}

			_dictItems.push_back(std::pair<std::string, IDataType*>(typeName, dataType));
		}

	}

	if(typeNode->subNodeExist("implementedBy")){
		 strType = (*typeNode)["implementedBy"][0].getValueString();
		 if(strType.size() > 0 && !loadImplModule(strType)){
			 printf("PyFixedDictType::initType: loadImplModule is error\n");
			 return false;
		 }
		 _moduleName = strType;
	}

	if(_dictItems.size() == 0){
		printf("FixedDictType::initType: FIXED_DICT(%s) no keys!\n", typeNode->name());
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
		ECHO_ERROR("fixedDictType::implCreateObjFromDict %s failed", _moduleName.c_str());
		SLASSERT(false, "wtf");
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

PyObject* PyFixedDictType::createNewItemFromObj(const char* keyName, PyObject* pyobj){
	int32 keyItemsNum = _dictItems.size();
	auto itor = _dictItems.begin();
	for(; itor != _dictItems.end(); ++itor){
		if(itor->first == keyName){
			return static_cast<PyDataType*>(itor->second)->createNewFromObj(pyobj);
		}
	}
	Py_RETURN_NONE;
}

PyObject* PyFixedDictType::createNewFromObj(PyObject* pyobj){
	if(hasImpl()){
		return implCreateObjFromDict(pyobj);
	}

	if(PyObject_TypeCheck(pyobj, FixedDict::getScriptType())){
		Py_INCREF(pyobj);
		return pyobj;
	}

	FixedDict* pFixedDict = NEW FixedDict(this);
	pFixedDict->initialize(pyobj);
	return pFixedDict;
}

void MailBoxType::addToStream(sl::IBStream& stream, void* value){
	PyObject* pyValue = (PyObject*)value;
	const char types[2][10] = {
			"Entity",
			"Base"
		};

	uint64 entityId = 0;
	int32 nodeId = 0;
	int16 mType = 0;
	int32 entityType = 0;
	if(pyValue != Py_None){
		for(int32 i =0; i < 2; i++){
			PyTypeObject* stype = sl::pyscript::ScriptObject::getScriptObjectType(types[i]); 
			if(stype == NULL)
				continue;
	
			if(PyObject_IsInstance(pyValue, (PyObject*)stype)){
				PyObject* pyid = PyObject_GetAttrString(pyValue, "id");
				if(pyid){
					entityId = PyLong_AsUnsignedLongLong(pyid);
					Py_DECREF(pyid);
				}
				else{
					SCRIPT_ERROR_CHECK();
					entityId = 0;
					nodeId = 0;
					break;
				}
			
				nodeId = SLMODULE(Harbor)->getNodeId();
				int32 nodeType = SLMODULE(Harbor)->getNodeType();
				if(nodeType == NodeType::LOGIC)
					mType = MAILBOX_TYPE_BASE;
				else if(nodeType == NodeType::SCENE)
					mType = MAILBOX_TYPE_CELL;
				else
					mType = MAILBOX_TYPE_CLIENT;
			
				EntityScriptObject* pyScriptEntity = static_cast<EntityScriptObject*>(pyValue);
				entityType = pyScriptEntity->getInnerObject()->getObjectType();
			}
		}

		if(entityId == 0){
			EntityMailBox* pMailBox = static_cast<EntityMailBox*>(pyValue);
			nodeId = pMailBox->getRemoteNodeId();
			entityId = pMailBox->getEntityId();
			mType = pMailBox->getType();
			entityType = pMailBox->getEntityType();
		}
	}

	stream << entityId;
	stream << nodeId;
	stream << mType;
	stream << entityType;
}

void* MailBoxType::createFromStream(const sl::OBStream& stream){
	uint64 entityId = 0;
	int32 nodeId = 0;
	int16 mType = 0;
	int32 entityType = 0;
	if(!stream.readUint64(entityId) || !stream.readInt32(nodeId) || !stream.readInt16(mType) || !stream.readInt32(entityType)){
			ECHO_ERROR("MailBoxType::createFromStream: read data error");
			return nullptr;
		}

	if(entityId > 0){
			PyObject* entity = EntityMailBox::tryGetEntity(mType, nodeId, entityId);
			if(entity){
						Py_INCREF(entity);
						return entity;
					}
	
			return NEW EntityMailBox(mType, nodeId, entityId, PythonServer::getInstance()->findScriptDefModule(entityType));
		}

	Py_RETURN_NONE;
}

const int32 MailBoxType::getSize() const{
	return sizeof(uint64) + 2 * sizeof(int32) + sizeof(int16);
}

