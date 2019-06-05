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
		//std::string dataTypeName = std::string("_") + sl::CStringUtils::Int64AsString(SLMODULE(IdMgr)->generateLocalId()) + typeName;
		//SLMODULE(ObjectDef)->addExtraDataType(dataTypeName.c_str(), fixedDict);
		return fixedDict;

	}else if(strcmp(typeName,"ARRAY") == 0){
		PyFixedArrayType* fixedArray = NEW PyFixedArrayType();
		if(!fixedArray->initType(type)){
			DEL fixedArray;
			return NULL;
		}
		//std::string dataTypeName = std::string("_") + sl::CStringUtils::Int64AsString(SLMODULE(IdMgr)->generateLocalId()) + typeName;
		//SLMODULE(ObjectDef)->addExtraDataType(dataTypeName.c_str(), fixedArray);
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

bool UInt64Type::addScriptObject(sl::IBStream& stream, void* value){
	uint64 val = 0;
	if(!addScriptObject(val, value))
		return false;

	stream << val;
	return true;
}

bool UInt64Type::addScriptObject(IObject* object, const IProp* prop, void* value){
	uint64 val = 0;
	if(!addScriptObject(val, value))
		return false;

	return object->setPropUint64(prop, val);
}

bool UInt64Type::addScriptObject(IArray* array, const int32 index, void* value){
	uint64 val = 0;
	if(!addScriptObject(val, value))
		return false;

	return array->setPropUint64(index, val);
}

void* UInt64Type::createScriptObject(const sl::OBStream& stream){
	uint64 val = 0;
	if(!stream.readUint64(val)){
		PyErr_Format(PyExc_TypeError, "UInt64Type::createScriptObject: read data from stream failed!");
		PyErr_PrintEx(0);

		return nullptr;
	}
	return createScriptObject(val);
}

void* UInt64Type::createScriptObject(IObject* object, const IProp* prop){
	uint64 val = object->getPropUint64(prop);
	return createScriptObject(val);
}

void* UInt64Type::createScriptObject(IArray* array, const int32 index){
	uint64 val = array->getPropUint64(index);
	return createScriptObject(val);
}

void* UInt64Type::parseDefaultStr(const char* defaultVal){
	uint64 val = sl::CStringUtils::StringAsUint64(defaultVal);
	return createScriptObject(val);
}

bool UInt64Type::addScriptObject(uint64& val, void* value){
	PyObject* pyValue = (PyObject*)value;
	val = static_cast<uint64>(PyLong_AsUnsignedLongLong(pyValue));
	if(PyErr_Occurred()){
		PyErr_Clear();
		PyErr_Format(PyExc_TypeError, "UInt64Type::addScriptObject: pyValue(%s) is wrong!",
				(pyValue == NULL) ? "NULL": pyValue->ob_type->tp_name);
		PyErr_PrintEx(0);

		return false;
	}

	return true;
}

void* UInt64Type::createScriptObject(uint64 val){
	PyObject* pyVal = PyLong_FromUnsignedLongLong(val);
	if(PyErr_Occurred()){
		PyErr_Clear();
		PyErr_Format(PyExc_TypeError, "UInt64Type::createScriptObject: errval=%lld", val);
		PyErr_PrintEx(0);
		if(pyVal){
			Py_DECREF(pyVal);
			pyVal = NULL;
		}
		return nullptr;
	}

	return pyVal;
}

bool UInt32Type::addScriptObject(sl::IBStream& stream, void* value){
	uint32 val = 0;
	if(!addScriptObject(val, value))
		return false;

	stream << val;
	return true;
}

bool UInt32Type::addScriptObject(IObject* object, const IProp* prop, void* value){
	uint32 val = 0;
	if(!addScriptObject(val, value))
		return false;

	return object->setPropUint32(prop, val);
}

bool UInt32Type::addScriptObject(IArray* array, const int32 index, void* value){
	uint32 val = 0;
	if(!addScriptObject(val, value))
		return false;

	return array->setPropUint32(index, val);
}

void* UInt32Type::createScriptObject(const sl::OBStream& stream){
	uint32 val = 0;
	if(!stream.readUint32(val)){
		PyErr_Format(PyExc_TypeError, "UInt32Type::createScriptObject: read data from buffer failed!"); 
		return nullptr;
	}

	return createScriptObject(val);
}

void* UInt32Type::createScriptObject(IObject* object, const IProp* prop){
	uint32 val = object->getPropUint32(prop);
	return createScriptObject(val);
}

void* UInt32Type::createScriptObject(IArray* array, const int32 index){
	uint32 val = array->getPropUint32(index);
	return createScriptObject(val);
}

bool UInt32Type::addScriptObject(uint32& val, void* value){
	PyObject* pyValue = (PyObject*)value;
	val = PyLong_AsUnsignedLong(pyValue);
	if(PyErr_Occurred()){
		PyErr_Clear();
		PyErr_Format(PyExc_TypeError, "UInt32Type::addScriptObject: pyValue(%s) is wrong!", 
				(pyValue == NULL) ? "NULL" : pyValue->ob_type->tp_name);
		PyErr_PrintEx(0);

		return false;
	}

	return true;
}

void* UInt32Type::createScriptObject(uint32 val){
	PyObject* pyVal = PyLong_FromUnsignedLong(val);
	if(PyErr_Occurred()){
		PyErr_Clear();
		PyErr_Format(PyExc_TypeError, "UInt32Type::createScriptObject: errval=%u", val);
		PyErr_PrintEx(0);
		if(pyVal){
			Py_DECREF(pyVal);
			pyVal = NULL;
		}
		return nullptr;
	}
	return pyVal;
}

void* UInt32Type::parseDefaultStr(const char* defaultValStr){
	uint32 val = (uint32)sl::CStringUtils::StringAsInt64(defaultValStr);
	return createScriptObject(val);
}

bool Int64Type::addScriptObject(sl::IBStream& stream, void* value){
	int64 val = 0;
	if(!addScriptObject(val, value))
		return false;

	stream << val;
	return true;
}

bool Int64Type::addScriptObject(IObject* object, const IProp* prop, void* value){
	int64 val = 0;
	if(!addScriptObject(val, value))
		return false;

	return object->setPropInt64(prop, val);
}

bool Int64Type::addScriptObject(IArray* array, const int32 index, void* value){
	int64 val = 0;
	if(!addScriptObject(val, value))
		return false;
	
	return array->setPropInt64(index, val);
}

void* Int64Type::createScriptObject(const sl::OBStream& stream){
	int64 val = 0;
	if(!stream.readInt64(val)){
		PyErr_Format(PyExc_TypeError, "Int64Type::createScriptObject: read data from buffer failed!");
		PyErr_PrintEx(0);
		return nullptr;
	}

	return createScriptObject(val);
}

void* Int64Type::createScriptObject(IObject* object, const IProp* prop){
	int64 val = object->getPropInt64(prop);
	return createScriptObject(val);
}

void* Int64Type::createScriptObject(IArray* array, const int32 index){
	int64 val = array->getPropInt64(index);
	return createScriptObject(val);
}

bool Int64Type::addScriptObject(int64& val, void* value){
	PyObject* pyValue = (PyObject*)value;
	val = PyLong_AsLongLong(pyValue);
	if(PyErr_Occurred()){
		PyErr_Clear();
		PyErr_Format(PyExc_TypeError, "Int64Type::addScriptObject: pyValue(%s) is wrong!",
				(pyValue == NULL) ? "NULL": pyValue->ob_type->tp_name);
		PyErr_PrintEx(0);

		return false;
	}

	return true;
}

void* Int64Type::createScriptObject(int64 val){
	PyObject* pyVal = PyLong_FromLongLong(val);
	if(PyErr_Occurred()){
		PyErr_Clear();
		PyErr_Format(PyExc_TypeError, "Int64Type::createScriptObject: errval=%lld", val);
		PyErr_PrintEx(0);
		if(pyVal){
			Py_DECREF(pyVal);
			pyVal = NULL;
		}

		return nullptr;
	}

	return pyVal;
}

void* Int64Type::parseDefaultStr(const char* defaultValStr){
	int64 val = sl::CStringUtils::StringAsInt64(defaultValStr);
	return createScriptObject(val);
}

bool FloatType::addScriptObject(sl::IBStream& stream, void* value){
	float val = 0;
	if(!addScriptObject(val, value))
		return false;

	stream << val;
	return true;
}

bool FloatType::addScriptObject(IObject* object, const IProp* prop, void* value){
	float val = 0;
	if(!addScriptObject(val, value))
		return false;

	return object->setPropFloat(prop, val);
}

bool FloatType::addScriptObject(IArray* array, const int32 index, void* value){
	float val = 0;
	if(!addScriptObject(val, value))
		return false;

	return array->setPropFloat(index, val);
}

void* FloatType::createScriptObject(const sl::OBStream& stream){
	float val = 0;
	if(!stream.readFloat(val)){
		PyErr_Format(PyExc_TypeError, "FloatType::createScriptObject: read data from stream failed!");
		PyErr_PrintEx(0);
		return nullptr;
	}

	return createScriptObject(val);
}

void* FloatType::createScriptObject(IObject* object, const IProp* prop){
	float val = object->getPropFloat(prop);
	return createScriptObject(val);
}

void* FloatType::createScriptObject(IArray* array, const int32 index){
	float val = array->getPropFloat(index);
	return createScriptObject(val);
}

bool FloatType::addScriptObject(float& val, void* value){
	PyObject* pyValue = (PyObject*)value;
	if(!PyFloat_Check(pyValue)){
		PyErr_Format(PyExc_TypeError, "FloatType::addScriptObject: pyValue(%s) is wrong!",
				(pyValue == NULL) ? "NULL" : pyValue->ob_type->tp_name);
		PyErr_PrintEx(0);

		return false;
	}
	
	val = (float)PyFloat_AsDouble(pyValue);
	return true;
}

void* FloatType::createScriptObject(float val){
	PyObject* pyVal = PyFloat_FromDouble(val);
	if(PyErr_Occurred()){
		PyErr_Clear();
		PyErr_Format(PyExc_TypeError, "FloatType::createScriptObject: errorval=%f", val);
		PyErr_PrintEx(0);
		if(pyVal){
			Py_DECREF(pyVal);
			pyVal = NULL;
		}
		return nullptr;
	}

	return pyVal;
}

void* FloatType::parseDefaultStr(const char* defaultStr){
	float val = sl::CStringUtils::StringAsFloat(defaultStr);
	return createScriptObject(val);
}

bool DoubleType::addScriptObject(sl::IBStream& stream, void* value){
	double val = 0;
	if(!addScriptObject(val, value))
		return false;

	stream << val;
	return true;
}

bool DoubleType::addScriptObject(IObject* object, const IProp* prop, void* value){
	double val = 0;
	if(!addScriptObject(val, value))
		return false;

	return object->setPropDouble(prop, val);
}

bool DoubleType::addScriptObject(IArray* array, const int32 index, void* value){
	double val = 0;
	if(!addScriptObject(val, value))
		return false;

	return array->setPropDouble(index, val);
}

void* DoubleType::createScriptObject(const sl::OBStream& stream){
	double val = 0.0;
	if(!stream.readDouble(val)){
		ECHO_ERROR("read DOUBLE from stream failed");
		return nullptr;
	}

	return createScriptObject(val);
}

void* DoubleType::createScriptObject(IObject* object, const IProp* prop){
	double val = object->getPropDouble(prop);
	return createScriptObject(val);
}

void* DoubleType::createScriptObject(IArray* array, const int32 index){
	double val = array->getPropDouble(index);
	return createScriptObject(val);
}

bool DoubleType::addScriptObject(double& val, void* value){
	PyObject* pyValue = (PyObject*)value;
	if(!PyFloat_Check(pyValue)){
		PyErr_Format(PyExc_TypeError, "DoubleType::addScriptObject: pyValue(%s) is wrong!",
				(pyValue == NULL) ? "NULL" : pyValue->ob_type->tp_name);
		PyErr_PrintEx(0);
		return false;
	}
	
	val =  PyFloat_AsDouble(pyValue);
	return true;
}

void* DoubleType::createScriptObject(double val){
	PyObject* pyVal = PyFloat_FromDouble(val);
	if(PyErr_Occurred()){
		PyErr_Clear();
		PyErr_Format(PyExc_TypeError, "DoubleType::createScriptObject: errval=%f", val);
		PyErr_PrintEx(0);
		if(pyVal){
			Py_DECREF(pyVal);
			pyVal = NULL;
		}
		return nullptr;;
	}
	return pyVal;
}

void* DoubleType::parseDefaultStr(const char* defaultValStr){
	double val = sl::CStringUtils::StringAsDouble(defaultValStr);
	return createScriptObject(val);
}

bool Vector2Type::addScriptObject(sl::IBStream& stream, void* value){
	float val[2] = {0.0f, 0.0f};
	if(!addScriptObject(val, value))
		return false;

	stream << val[0];
	stream << val[1];
	return true;
}

bool Vector2Type::addScriptObject(IObject* object, const IProp* prop, void* value){
	float val[2] = {0.0f, 0.0f};
	if(!addScriptObject(val, value))
		return false;

	return object->setPropBlob(prop, (const void*)val, sizeof(float) * 2);
}

bool Vector2Type::addScriptObject(IArray* array, const int32 index, void* value){
	float val[2] = {0.0f, 0.0f};
	if(!addScriptObject(val, value))
		return false;

	return array->setPropBlob(index, (const void*)val, sizeof(float) * 2);
}

void* Vector2Type::createScriptObject(const sl::OBStream& stream){
	float x =0.0f, y = 0.0f;
	if(!stream.readFloat(x) || !stream.readFloat(y)){
		PyErr_Format(PyExc_TypeError, "Vector2Type::createScriptObject: read data from stream failed!");
		PyErr_PrintEx(0);
		return nullptr;
	}

	float val[2] = {x, y};
	return createScriptObject(val);
}

void* Vector2Type::createScriptObject(IObject* object, const IProp* prop){
	int32 size = 0;
	const void* val = object->getPropBlob(prop, size);
	if(size != sizeof(float) * 2){
		PyErr_Format(PyExc_TypeError, "Vector2Type::createScriptObject: read data from stream failed!");
		PyErr_PrintEx(0);
		return nullptr;
	}

	return createScriptObject((float*)val);
	//return (PyObject*)(NEW sl::pyscript::ScriptVector2(ptr[0], ptr[1]));
}

void* Vector2Type::createScriptObject(IArray* array, const int32 index){
	int32 size = 0;
	const void* val = array->getPropBlob(index, size);
	if(size != sizeof(float) * 2){
		PyErr_Format(PyExc_TypeError, "Vector2Type::createScriptObject: read data from stream failed!");
		PyErr_PrintEx(0);
		return nullptr;
	}

	return createScriptObject((float*)val);
}

bool Vector2Type::addScriptObject(float* val, void* value){
	PyObject* pyValue = (PyObject*)value;
	for(int32 index = 0; index < 2; index++){
		PyObject* pyVal = PySequence_GetItem(pyValue, index);
		if(PyErr_Occurred()){
			PyErr_Clear();
			PyErr_Format(PyExc_TypeError, "Vector2Type::addScriptObject: add index %d failed", index);
			PyErr_PrintEx(0);
			if(pyVal){
				Py_DECREF(pyVal);
				pyVal = NULL;
			}
			return false;
		}	

		val[index] = (float)PyFloat_AsDouble(pyVal);
		Py_DECREF(pyVal);
	}
	
	return true;
}

void* Vector2Type::createScriptObject(float* val){
	PyObject* pyObj = PyTuple_New(2);
	for(int32 index = 0; index < 2; index++){
		PyObject* pyVal = PyFloat_FromDouble(val[index]);
		if(PyErr_Occurred()){
			PyErr_Clear();
			PyErr_Format(PyExc_TypeError, "DoubleType::createScriptObject: errval=%f", val[index]);
			PyErr_PrintEx(0);
			if(pyVal){
				Py_DECREF(pyVal);
				pyVal = NULL;
			}

			if(pyObj){
				Py_DECREF(pyObj);
				pyObj = NULL;
			}
			return nullptr;;
		}

		PyTuple_SetItem(pyObj, index, pyVal);
	}

	return pyObj;
}

void* Vector2Type::parseDefaultStr(const char* defaultValStr){
	float x = 0.0f, y = 0.0f;
	if(!defaultValStr && strcmp(defaultValStr, "") != 0){
		std::stringstream sstream;
		sstream << defaultValStr;
		sstream >> x >> y;
	}

	float val[2] = {x, y};
	return createScriptObject(val);
}

bool Vector3Type::addScriptObject(sl::IBStream& stream, void* value){
	float val[3] = {0.0f, 0.0f, 0.0f};
	if(!addScriptObject(val, value))
		return false;

	stream << val[0];
	stream << val[1];
	stream << val[2];
	return true;
}

bool Vector3Type::addScriptObject(IObject* object, const IProp* prop, void* value){
	float val[3] = {0.0f, 0.0f, 0.0f};
	if(!addScriptObject(val, value))
		return false;

	return object->setPropBlob(prop, (const void*)val, sizeof(float) * 3);
}

bool Vector3Type::addScriptObject(IArray* array, const int32 index, void* value){
	float val[3] = {0.0f, 0.0f, 0.0f};
	if(!addScriptObject(val, value))
		return false;

	return array->setPropBlob(index, (const void*)val, sizeof(float) * 3);
}

void* Vector3Type::createScriptObject(const sl::OBStream& stream){
	float x = 0.0f, y = 0.0f, z = 0.0f;
	if(!stream.readFloat(x) || !stream.readFloat(y) || !stream.readFloat(z)){
		PyErr_Format(PyExc_TypeError, "Vector3Type::addScriptObject: read data from stream failed!");
		PyErr_PrintEx(0);
		return nullptr;
	}

	float val[3] = {x, y, z};
	return createScriptObject(val);
}

void* Vector3Type::createScriptObject(IObject* object, const IProp* prop){
	int32 size = 0;
	const void* val = object->getPropBlob(prop, size);
	if(size != sizeof(float) * 3){
		PyErr_Format(PyExc_TypeError, "Vector3Type::createScriptObject: read data from stream failed!");
		PyErr_PrintEx(0);
		return nullptr;
	}

	return createScriptObject((float*)val);
}

void* Vector3Type::createScriptObject(IArray* array, const int32 index){
	int32 size = 0;
	const void* val = array->getPropBlob(index, size);
	if(size != sizeof(float) * 3){
		PyErr_Format(PyExc_TypeError, "Vector3Type::createScriptObject: read data from stream failed!");
		PyErr_PrintEx(0);
		return nullptr;
	}

	return createScriptObject((float*)val);
}

bool Vector3Type::addScriptObject(float* val, void* value){
	PyObject* pyValue = (PyObject*)value;
	for(int32 index = 0; index < 3; index++){
		PyObject* pyVal = PySequence_GetItem(pyValue, index);
		if(PyErr_Occurred()){
			PyErr_Clear();
			PyErr_Format(PyExc_TypeError, "Vector2Type::addScriptObject: add index %d failed", index);
			PyErr_PrintEx(0);
			if(pyVal){
				Py_DECREF(pyVal);
				pyVal = NULL;
			}
			return false;
		}	

		val[index] = (float)PyFloat_AsDouble(pyVal);
		Py_DECREF(pyVal);
	}
	
	return true;
}

void* Vector3Type::createScriptObject(float* val){
	PyObject* pyObj = PyTuple_New(3);
	for(int32 index = 0; index < 3; index++){
		PyObject* pyVal = PyFloat_FromDouble(val[index]);
		if(PyErr_Occurred()){
			PyErr_Clear();
			PyErr_Format(PyExc_TypeError, "DoubleType::createScriptObject: errval=%f", val[index]);
			PyErr_PrintEx(0);
			if(pyVal){
				Py_DECREF(pyVal);
				pyVal = NULL;
			}

			if(pyObj){
				Py_DECREF(pyObj);
				pyObj = NULL;
			}
			return nullptr;;
		}

		PyTuple_SetItem(pyObj, index, pyVal);
	}

	return pyObj;
}

void* Vector3Type::parseDefaultStr(const char* defaultValStr){
	float x = 0.0f, y = 0.0f, z = 0.0f;
	if(!defaultValStr && strcmp(defaultValStr, "") != 0){
		std::stringstream sstream;
		sstream << defaultValStr;
		sstream >> x >> y >> z;
	}

	float val[3] = {x, y, z};
	return createScriptObject((float*)val);
}

bool Vector4Type::addScriptObject(sl::IBStream& stream, void* value){
	float val[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	if(!addScriptObject(val, value))
		return false;

	stream << val[0];
	stream << val[1];
	stream << val[2];
	stream << val[3];
	return true;

}

bool Vector4Type::addScriptObject(IObject* object, const IProp* prop, void* value){
	float val[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	if(!addScriptObject(val, value))
		return false;

	return object->setPropBlob(prop, (const void*)val, sizeof(float) * 4);
}

bool Vector4Type::addScriptObject(IArray* array, const int32 index, void* value){
	float val[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	if(!addScriptObject(val, value))
		return false;

	return array->setPropBlob(index, (const void*)val, sizeof(float) * 4);
}

void* Vector4Type::createScriptObject(const sl::OBStream& stream){
	float x = 0.0f, y = 0.0f, z = 0.0f, t = 0.0f;
	if(!stream.readFloat(x) || !stream.readFloat(y) || !stream.readFloat(z) || !stream.readFloat(t)){
		PyErr_Format(PyExc_TypeError, "Vector4Type::addScriptObject: read data from stream failed!");
		PyErr_PrintEx(0);

		return nullptr;
	}

	float val[4] = {x, y, z, t};
	return createScriptObject(val);
}

void* Vector4Type::createScriptObject(IObject* object, const IProp* prop){
	int32 size = 0;
	const void* val = object->getPropBlob(prop, size);
	if(size != sizeof(float) * 4){
		PyErr_Format(PyExc_TypeError, "Vector4Type::createScriptObject: read data from stream failed!");
		PyErr_PrintEx(0);
		return nullptr;
	}

	return createScriptObject((float*)val);
}

void* Vector4Type::createScriptObject(IArray* array, const int32 index){
	int32 size = 0;
	const void* val = array->getPropBlob(index, size);
	if(size != sizeof(float) * 4){
		PyErr_Format(PyExc_TypeError, "Vector4Type::createScriptObject: read data from stream failed!");
		PyErr_PrintEx(0);
		return nullptr;
	}

	return createScriptObject((float*)val);
}

bool Vector4Type::addScriptObject(float* val, void* value){
	PyObject* pyValue = (PyObject*)value;
	for(int32 index = 0; index < 4; index++){
		PyObject* pyVal = PySequence_GetItem(pyValue, index);
		if(PyErr_Occurred()){
			PyErr_Clear();
			PyErr_Format(PyExc_TypeError, "Vector2Type::addScriptObject: add index %d failed", index);
			PyErr_PrintEx(0);
			if(pyVal){
				Py_DECREF(pyVal);
				pyVal = NULL;
			}
			return false;
		}	

		val[index] = (float)PyFloat_AsDouble(pyVal);
		Py_DECREF(pyVal);
	}
	
	return true;
}

void* Vector4Type::createScriptObject(float* val){
	PyObject* pyObj = PyTuple_New(4);
	for(int32 index = 0; index < 4; index++){
		PyObject* pyVal = PyFloat_FromDouble(val[index]);
		if(PyErr_Occurred()){
			PyErr_Clear();
			PyErr_Format(PyExc_TypeError, "DoubleType::createScriptObject: errval=%f", val[index]);
			PyErr_PrintEx(0);
			if(pyVal){
				Py_DECREF(pyVal);
				pyVal = NULL;
			}

			if(pyObj){
				Py_DECREF(pyObj);
				pyObj = NULL;
			}
			return nullptr;;
		}

		PyTuple_SetItem(pyObj, index, pyVal);
	}

	return pyObj;
}

void* Vector4Type::parseDefaultStr(const char* defaultValStr){
	float x= 0.0f, y = 0.0f, z = 0.0f, t = 0.0f;
	if(!defaultValStr && strcmp(defaultValStr, "") != 0){
		std::stringstream sstream;
		sstream << defaultValStr;
		sstream >> x >> y >> z >> t;
	}

	float val[4] = {x, y, z, t};
	return createScriptObject(val);
}

bool StringType::addScriptObject(sl::IBStream& stream, void* value){
	char* val = addScriptObject(value);
	if(!val){
		return false;
	}

	stream << val;
	free(val);
	return true;
}

bool StringType::addScriptObject(IObject* object, const IProp* prop, void* value){
	char* val = addScriptObject(value);
	if(!val){
		return false;
	}

	bool ret = object->setPropString(prop, val);
	free(val);
	return ret;
}

bool StringType::addScriptObject(IArray* array, const int32 index, void* value){
	char* val = addScriptObject(value);
	if(!val){
		return false;
	}

	bool ret = array->setPropString(index, val);
	free(val);
	return ret;
}

void* StringType::createScriptObject(const sl::OBStream& stream){
	const char* data = nullptr;
	if(!stream.readString(data)){
		PyErr_Format(PyExc_TypeError, "StringType::createScriptObject: read data from stream failed!");
		PyErr_PrintEx(0);
		return nullptr;
	}

	return createScriptObject(data);
}

void* StringType::createScriptObject(IObject* object, const IProp* prop){
	const char* data = object->getPropString(prop);
	return createScriptObject(data);
}

void* StringType::createScriptObject(IArray* array, const int32 index){
	const char* data = array->getPropString(index);
	return createScriptObject(data);
}

char* StringType::addScriptObject(void* value){
	PyObject* pyValue = (PyObject*)value;
	wchar_t* pyUnicode_AsWideCharStringRet0 = PyUnicode_AsWideCharString(pyValue, NULL);

	size_t size = 0;
	char* ret = sl::CStringUtils::wchar2char(pyUnicode_AsWideCharStringRet0, &size);
	PyMem_Free(pyUnicode_AsWideCharStringRet0);

	if(!ret){
		PyErr_Format(PyExc_TypeError, "StringType::addScriptObject: stream buffer size is not enough!");
		PyErr_PrintEx(0);
	}
	return ret;
}

void* StringType::createScriptObject(const char* data){
	PyObject* pyObj = PyUnicode_FromString((const char*)data);
	if(PyErr_Occurred()){
		PyErr_Clear();
		PyErr_Format(PyExc_TypeError, "StringType::createScriptObject: error value = %s!", data);
		PyErr_PrintEx(0);

		if(pyObj){
			Py_DECREF(pyObj);
			pyObj = NULL;
		}
		return nullptr;
	}

	return pyObj;
}

void* StringType::parseDefaultStr(const char* defaultValStr){
	if(!defaultValStr){
		defaultValStr = "";
	}

	return createScriptObject(defaultValStr);
}

bool UnicodeType::addScriptObject(sl::IBStream& stream, void* value){
	PyObject* pyObj = PyUnicode_AsUTF8String((PyObject*)value);
	if(pyObj == NULL){
		PyErr_Format(PyExc_TypeError, "UnicodeType::addScriptObject: PyUniocde_AsUTF8String failed!");
		PyErr_PrintEx(0);

		return false;
	}

	stream.addBlob(PyBytes_AS_STRING(pyObj), (int32)PyBytes_GET_SIZE(pyObj));
	Py_DECREF(pyObj);

	return true;
}

bool UnicodeType::addScriptObject(IObject* object, const IProp* prop, void* value){
	PyObject* pyObj = PyUnicode_AsUTF8String((PyObject*)value);
	if(pyObj == NULL){
		PyErr_Format(PyExc_TypeError, "UnicodeType::addScriptObject: PyUniocde_AsUTF8String failed!");
		PyErr_PrintEx(0);

		return false;
	}

	return object->setPropBlob(prop, PyBytes_AS_STRING(pyObj), (int32)PyBytes_GET_SIZE(pyObj));
}

bool UnicodeType::addScriptObject(IArray* array, const int32 index, void* value){
	PyObject* pyObj = PyUnicode_AsUTF8String((PyObject*)value);
	if(pyObj == NULL){
		PyErr_Format(PyExc_TypeError, "UnicodeType::addScriptObject: PyUniocde_AsUTF8String failed!");
		PyErr_PrintEx(0);

		return false;
	}

	return array->setPropBlob(index, PyBytes_AS_STRING(pyObj), (int32)PyBytes_GET_SIZE(pyObj));
}

void* UnicodeType::createScriptObject(const sl::OBStream& stream){
	int32 size = 0;
	const void* data = stream.readBlob(size);	
	if(!data){
		PyErr_Format(PyExc_TypeError, "UnicodeType::createScriptObject: read data from stream failed!");
		PyErr_PrintEx(0);
		return nullptr;
	}

	return createScriptObject(data, size);
}

void* UnicodeType::createScriptObject(IObject* object, const IProp* prop){
	int32 size = 0;
	const void* data = object->getPropBlob(prop, size);
	return createScriptObject(data, size);
}

void* UnicodeType::createScriptObject(IArray* array, const int32 index){
	int32 size = 0;
	const void* data = array->getPropBlob(index, size);
	return createScriptObject(data, size);
}

void* UnicodeType::createScriptObject(const void* data, const int32 size){
	PyObject* pyObj = PyUnicode_DecodeUTF8((const char*)data, size, "");
	if(PyErr_Occurred()){
		PyErr_Clear();
		PyErr_Format(PyExc_TypeError, "UnicodeType::createScriptObject: PyUnicode_DecodeUTF8 failed!");
		PyErr_PrintEx(0);

		if(pyObj){
			Py_DECREF(pyObj);
			pyObj = NULL;
		}

		return nullptr;
	}
	
	return pyObj;
}

void* UnicodeType::parseDefaultStr(const char* defaultValStr){
	if(!defaultValStr){
		defaultValStr = "";
	}

	return createScriptObject(defaultValStr, strlen(defaultValStr) + 1);
}

bool BlobType::addScriptObject(sl::IBStream& stream, void* value){
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

	return true;
}

bool BlobType::addScriptObject(IObject* object, const IProp* prop, void* value){
	PyObject* pyValue = (PyObject*)value;
	if(!PyBytes_Check(pyValue)){
		sl::pyscript::PyMemoryStream* pPyMemoryStream = static_cast<sl::pyscript::PyMemoryStream*>(pyValue);
		sl::MemoryStream& m = pPyMemoryStream->stream();
		object->setPropBlob(prop, (const char*)m.data() + m.rpos(), (int32)m.length());
	}
	else{
		Py_ssize_t dataSize = PyBytes_GET_SIZE(pyValue);
		char* datas = PyBytes_AsString(pyValue);
		object->setPropBlob(prop, datas, (int32)dataSize);
	}

	return true;
}

bool BlobType::addScriptObject(IArray* array, const int32 index, void* value){
	PyObject* pyValue = (PyObject*)value;
	if(!PyBytes_Check(pyValue)){
		sl::pyscript::PyMemoryStream* pPyMemoryStream = static_cast<sl::pyscript::PyMemoryStream*>(pyValue);
		sl::MemoryStream& m = pPyMemoryStream->stream();
		array->setPropBlob(index, (const char*)m.data() + m.rpos(), (int32)m.length());
	}
	else{
		Py_ssize_t dataSize = PyBytes_GET_SIZE(pyValue);
		char* datas = PyBytes_AsString(pyValue);
		array->setPropBlob(index, datas, (int32)dataSize);
	}

	return true;
}

void* BlobType::createScriptObject(const sl::OBStream& stream){
	int32 size = 0;
	const char* data = (const char*)stream.readBlob(size);
	if(!data){
		PyErr_Format(PyExc_TypeError, "BlobType::createScriptObject: read data from stream failed!");
		PyErr_PrintEx(0);
		return nullptr;
	}
	
	return createScriptObject(data, size);
}

void* BlobType::createScriptObject(IObject* object, const IProp* prop){
	int32 size = 0;
	const void* data = object->getPropBlob(prop, size);

	return createScriptObject(data, size);
}

void* BlobType::createScriptObject(IArray* array, const int32 index){
	int32 size = 0;
	const void* data = array->getPropBlob(index, size);

	return createScriptObject(data, size);
}

void* BlobType::createScriptObject(const void* data, const int32 size){
	PyObject* pyObj = PyBytes_FromStringAndSize(const_cast<char*>((const char*)data), size);
	if(PyErr_Occurred()){
		PyErr_Clear();
		PyErr_Format(PyExc_TypeError, "BlobType::createScriptObject: PyBytes_FromStringAndSize failed!");
		PyErr_PrintEx(0);

		if(pyObj){
			Py_DECREF(pyObj);
			pyObj = NULL;
		}

		return nullptr;
	}

	return pyObj;
}

void* BlobType::parseDefaultStr(const char* defaultValStr){
	if(defaultValStr){
		defaultValStr = "";
	}
	return createScriptObject(defaultValStr, strlen(defaultValStr)); 
}

bool PythonType::addScriptObject(sl::IBStream& stream, void* value){
	PyObject* pyValue = (PyObject*)value;
	std::string datas = SLMODULE(PythonEngine)->pickle(pyValue);
	stream.addBlob(datas.data(), datas.size());

	return true;
}

bool PythonType::addScriptObject(IObject* object, const IProp* prop, void* value){
	PyObject* pyValue = (PyObject*)value;
	std::string datas = SLMODULE(PythonEngine)->pickle(pyValue);

	return object->setPropBlob(prop, datas.data(), datas.size());
}

bool PythonType::addScriptObject(IArray* array, const int32 index, void* value){
	PyObject* pyValue = (PyObject*)value;
	std::string datas = SLMODULE(PythonEngine)->pickle(pyValue);

	return array->setPropBlob(index, datas.data(), datas.size());
}

void* PythonType::createScriptObject(const sl::OBStream& stream){
	int32 size = 0;
	const void* data = stream.readBlob(size);
	if(!data){
		PyErr_Format(PyExc_TypeError, "PythonType::createScriptObject: read data from stream failed!");
		PyErr_PrintEx(0);
		return nullptr;
	}

	std::string datas((const char*)data, size);
	return SLMODULE(PythonEngine)->unpickle(datas);
}

void* PythonType::createScriptObject(IObject* object, const IProp* prop){
	int32 size = 0;
	const void* data = object->getPropBlob(prop, size);

	std::string datas((const char*)data, size);
	return SLMODULE(PythonEngine)->unpickle(datas);
}

void* PythonType::createScriptObject(IArray* array, const int32 index){
	int32 size = 0;
	const void* data = array->getPropBlob(index, size);

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

bool PyFixedArrayType::addScriptObject(sl::IBStream& stream, void* value){
	uint32 size = (uint32)PySequence_Size((PyObject*)value);
	stream << size;

	for(uint32 i = 0; i < size; i++){
		PyObject* pyVal = PySequence_GetItem((PyObject*)value, i);
		if(!_dataType->addScriptObject(stream, pyVal))
			return false;
	}

	return true;
}

bool PyFixedArrayType::addScriptObject(IObject* object, const IProp* prop, void* value){
	uint32 size = (uint32)PySequence_Size((PyObject*)value);
	IArray* objArray = object->getPropArray(prop);

	for(uint32 i = 0; i < size; i++){
		PyObject* pyVal = PySequence_GetItem((PyObject*)value, i);
		if(!_dataType->addScriptObject(objArray, i, pyVal))
			return false;
	}

	return true;
}

bool PyFixedArrayType::addScriptObject(IArray* array, const int32 index, void* value){
	uint32 size = (uint32)PySequence_Size((PyObject*)value);
	IArray* objArray = array->getPropArray(index);

	for(uint32 i = 0; i < size; i++){
		PyObject* pyVal = PySequence_GetItem((PyObject*)value, i);
		if(!_dataType->addScriptObject(objArray, i, pyVal))
			return false;
	}

	return true;
}

void* PyFixedArrayType::createScriptObject(const sl::OBStream& stream){
	uint32 size = 0;
	if(!stream.readUint32(size)){
		PyErr_SetString(PyExc_SystemError, "PyFixedArrayType::createScriptObject: read data from stream failed");
		PyErr_PrintEx(0);
		return nullptr;
	}

	PyObject* pyObj = PyList_New(size);
	for(int32 i = 0; i < size; i++){
		PyObject* pyVal = (PyObject*)(_dataType->createScriptObject(stream));
		if(!pyVal){
			PyErr_SetString(PyExc_SystemError, "PyFixedArrayType::createScriptObject: create array item from stream failed");
			PyErr_PrintEx(0);

			if(pyObj){
				Py_DECREF(pyObj);
				pyObj = NULL;
			}

			return nullptr;
		}
		
		PyList_SetItem(pyObj, i, pyVal);
	}
	return pyObj;
	
}

void* PyFixedArrayType::createScriptObject(IObject* object, const IProp* prop){
	IArray* objArray = object->getPropArray(prop);
	return NEW FixedArray(this, objArray);
}

void* PyFixedArrayType::createScriptObject(IArray* array, const int32 index){
	IArray* objArray = array->getPropArray(index);
	return NEW FixedArray(this, objArray);
}

void* PyFixedArrayType::parseDefaultStr(const char* defaultValStr){
	return PyList_New(0);
}

PyObject* PyFixedArrayType::createNewItemFromObj(PyObject* pyobj){
	return static_cast<PyDataType*>(_dataType)->createNewFromObj(pyobj);
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
			_typeSize += dataType->getSize();
		}
		else{
			IDataType* dataType = SLMODULE(ObjectDef)->getDataType(strType.c_str());
			if(!dataType){
				printf("PyFixedDictType::initType: key[%s] dont find type[%s]\n", typeName.c_str(), strType.c_str());
				return false;
			}

			_dictItems.push_back(std::pair<std::string, IDataType*>(typeName, dataType));
			_typeSize += dataType->getSize();
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

bool PyFixedDictType::addScriptObject(sl::IBStream& stream, void* value){
	/*
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
*/
	return true;
}

void* PyFixedDictType::createScriptObject(const sl::OBStream& stream){
	/*
	FixedDict* pyDict = NEW FixedDict(this, false);
	pyDict->initialize(stream, false);
	if(hasImpl()){
		PyObject* pyValue = implCreateObjFromDict(pyDict);
		Py_DECREF(pyDict);
		return pyValue;
	}

	return (PyObject*)pyDict;
	*/
	return nullptr;
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

bool MailBoxType::addScriptObject(sl::IBStream& stream, void* value){
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

void* MailBoxType::createScriptObject(const sl::OBStream& stream){
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

