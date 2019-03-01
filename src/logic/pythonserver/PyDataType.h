#ifndef SL_LOGIC_PY_DATA_TYPE_H
#define SL_LOGIC_PY_DATA_TYPE_H
#include "slmulti_sys.h"
#include "Python.h"
#include "IObjectDef.h"
#include "IDCCenter.h"
#include "slmemorystream.h"
#include "slxml_reader.h"

/*所有的python数据类型处理类*/
class PyDataType: public IDataType{
public:
	PyDataType()
		:_id(0), 
		_aliasName("")
	{}

	virtual ~PyDataType(){}

	virtual bool initType(sl::ISLXmlNode* typeNode) {return true;}
	virtual void setUid(const uint16 id) {_id = id;}
	virtual const uint16 getUid() const {return _id;}
	virtual void setAliasName(const char* aliasName) {_aliasName = aliasName;}
	virtual const char* getAliasName() const {return _aliasName.c_str();}
	virtual const int32 getSize() const {return 256;}	
	virtual void addDataTypeInfo(sl::IBStream& stream);
	virtual IDataType* arrayDataType() {return NULL;}
	virtual std::vector<std::pair<string, IDataType*>> dictDataType() {static std::vector<std::pair<string, IDataType*>> empty; return empty;}
	virtual PyObject* createNewItemFromObj(PyObject* pyobj){ Py_INCREF(pyobj); return pyobj; }
	virtual PyObject* createNewFromObj(PyObject* pyobj) { Py_INCREF(pyobj); return pyobj; }
	static IDataType* createDataType(const char* typeName, const sl::ISLXmlNode* typeNode);
	static IDataType* createDataType(const char* typeName);
	static bool initialize();

public:
	uint16			_id;
	std::string		_aliasName;
	static std::unordered_map<std::string, IDataType*> s_pyDataTypes;
};

template <typename SPECIFY_TYPE>
class IntType: public PyDataType{
public:
	virtual ~IntType(){}
	virtual const char* getName() const {return "INT";}
	virtual const int32 getSize() const {return sizeof(int32);}
	virtual const int8 getType() const {return DTYPE_INT32;}
	virtual bool addScriptObject(sl::IBStream& stream, void* value);
	virtual bool addScriptObject(IObject* object, const IProp* prop, void* value);
	virtual bool addScriptObject(IArray* array, const int32 index, void* value);
	virtual void* createScriptObject(const sl::OBStream& stream);
	virtual void* createScriptObject(IObject* object, const IProp* prop);
	virtual void* createScriptObject(IArray* array, const int32 index);
	virtual void* parseDefaultStr(const char* defaultValStr);

protected:
	bool addScriptObject(SPECIFY_TYPE& val, void* value);
	void* createScriptObject(SPECIFY_TYPE val);
};

template <>
inline const char* IntType<uint8>::getName()const {return "UINT8";}
template <>
inline const int32 IntType<uint8>::getSize()const {return sizeof(uint8);}
template <>
inline const int8 IntType<uint8>::getType() const {return DTYPE_UINT8;}

template <>
inline const char* IntType<uint16>::getName()const {return "UINT16";}
template <>
inline const int32 IntType<uint16>::getSize()const {return sizeof(uint16);}
template <>
inline const int8 IntType<uint16>::getType() const {return DTYPE_UINT16;}

template <>
inline const char* IntType<uint32>::getName() const {return "UINT32";}
template <>
inline const int32 IntType<uint32>::getSize()const {return sizeof(uint32);}
template <>
inline const int8 IntType<uint32>::getType() const {return DTYPE_UINT32;}

template <>
inline const char* IntType<int8>::getName() const {return "INT8";}
template <>
inline const int32 IntType<int8>::getSize()const {return sizeof(int8);}
template <>
inline const int8 IntType<int8>::getType() const {return DTYPE_INT8;}

template <>
inline const char* IntType<int16>::getName() const { return "INT16";}
template <>
inline const int32 IntType<int16>::getSize()const {return sizeof(int16);}
template <>
inline const int8 IntType<int16>::getType() const {return DTYPE_INT16;}

template <>
inline const char* IntType<int32>::getName() const {return "INT32";}
template <>
inline const int32 IntType<int32>::getSize()const {return sizeof(int32);}
template <>
inline const int8 IntType<int32>::getType() const {return DTYPE_INT32;}

template class IntType<int8>;
template class IntType<int16>;
template class IntType<uint8>;
template class IntType<uint16>;
template class IntType<int32>;

template <typename SPECIFY_TYPE>
bool IntType<SPECIFY_TYPE>::addScriptObject(SPECIFY_TYPE& val, void* value){
	PyObject* pyValue = (PyObject*)value;
	val = (SPECIFY_TYPE)PyLong_AsLong(pyValue);
	if(PyErr_Occurred()){
		PyErr_Clear();

		val = (SPECIFY_TYPE)PyLong_AsUnsignedLong(pyValue);
		if(PyErr_Occurred()){
			PyErr_Clear();
			PyErr_Format(PyExc_TypeError, "IntType::addScriptObject: pyValue(%s) is wrong!",
					(pyValue == NULL) ? "NULL" : pyValue->ob_type->tp_name);
			PyErr_PrintEx(0);

			val = 0;
			return false;
		}
	}
	return true;
}

template <typename SPECIFY_TYPE>
bool IntType<SPECIFY_TYPE>::addScriptObject(sl::IBStream& stream, void* value){
	SPECIFY_TYPE val = 0;
	if(!addScriptObject(val, value))
		return false;

	stream << val;
	return true;
}

template <>
bool IntType<int8>::addScriptObject(IObject* object, const IProp* prop, void* value){
	int8 val = 0;
	if(!addScriptObject(val, value))
		return false;

	return object->setPropInt8(prop, val);
}

template <>
bool IntType<int16>::addScriptObject(IObject* object, const IProp* prop, void* value){
	int16 val = 0;
	if(!addScriptObject(val, value))
		return false;

	return object->setPropInt16(prop, val);
}

template <>
bool IntType<uint8>::addScriptObject(IObject* object, const IProp* prop, void* value){
	uint8 val = 0;
	if(!addScriptObject(val, value))
		return false;

	return object->setPropUint8(prop, val);
}

template <>
bool IntType<uint16>::addScriptObject(IObject* object, const IProp* prop, void* value){
	uint16 val = 0;
	if(!addScriptObject(val, value))
		return false;

	return object->setPropUint16(prop, val);
}

template <>
bool IntType<int32>::addScriptObject(IObject* object, const IProp* prop, void* value){
	int32 val = 0;
	if(!addScriptObject(val, value))
		return false;

	return object->setPropInt32(prop, val);
}

template <>
bool IntType<int8>::addScriptObject(IArray* array, const int32 index, void* value){
	int8 val = 0;
	if(!addScriptObject(val, value))
		return false;

	return array->setPropInt8(index, val);
}

template <>
bool IntType<int16>::addScriptObject(IArray* array, const int32 index, void* value){
	int16 val = 0;
	if(!addScriptObject(val, value))
		return false;

	return array->setPropInt16(index, val);
}

template <>
bool IntType<uint8>::addScriptObject(IArray* array, const int32 index, void* value){
	uint8 val = 0;
	if(!addScriptObject(val, value))
		return false;

	return array->setPropUint8(index, val);
}

template <>
bool IntType<uint16>::addScriptObject(IArray* array, const int32 index, void* value){
	uint16 val = 0;
	if(!addScriptObject(val, value))
		return false;

	return array->setPropUint16(index, val);
}

template <>
bool IntType<int32>::addScriptObject(IArray* array, const int32 index, void* value){
	int32 val = 0;
	if(!addScriptObject(val, value))
		return false;

	return array->setPropInt32(index, val);
}

template <>
void* IntType<int8>::createScriptObject(const sl::OBStream& stream){
	int8 value = 0;
	if(!stream.readInt8(value)){
		PyErr_Format(PyExc_TypeError, "IntType::createScriptObject: read buffer data(int8) occur wrong!");
		PyErr_PrintEx(0);
		return nullptr;
	}
	return createScriptObject(value); 
}

template <>
void* IntType<int16>::createScriptObject(const sl::OBStream& stream){
	int16 value = 0;
	if(!stream.readInt16(value)){
		PyErr_Format(PyExc_TypeError, "IntType::createScriptObject: read buffer data(int16) occur wrong!");
		PyErr_PrintEx(0);
		return nullptr;
	}
	return createScriptObject(value); 
}

template <>
void* IntType<uint8>::createScriptObject(const sl::OBStream& stream){
	uint8 value = 0;
	if(!stream.readUint8(value)){
		PyErr_Format(PyExc_TypeError, "IntType::createScriptObject: read buffer data(uint8) occur wrong!");
		PyErr_PrintEx(0);
		return nullptr;
	}
	return createScriptObject(value); 
}

template <>
void* IntType<uint16>::createScriptObject(const sl::OBStream& stream){
	uint16 value = 0;
	if(!stream.readUint16(value)){
		PyErr_Format(PyExc_TypeError, "IntType::createScriptObject: read buffer data(uint16) occur wrong!");
		PyErr_PrintEx(0);
		return nullptr;
	}
	return createScriptObject(value); 
}

template <>
void* IntType<int32>::createScriptObject(const sl::OBStream& stream){
	int32 value = 0;
	if(!stream.readInt32(value)){
		PyErr_Format(PyExc_TypeError, "IntType::createScriptObject: read buffer data(int32) occur wrong!");
		PyErr_PrintEx(0);
		return nullptr;
	}
	return createScriptObject(value); 
}

template <>
void* IntType<int8>::createScriptObject(IObject* object, const IProp* prop){
	int8 value = object->getPropInt8(prop);
	return createScriptObject(value); 
}

template <>
void* IntType<int16>::createScriptObject(IObject* object, const IProp* prop){
	int16 value = object->getPropInt16(prop);
	return createScriptObject(value); 
}

template <>
void* IntType<uint8>::createScriptObject(IObject* object, const IProp* prop){
	uint8 value = object->getPropUint8(prop);
	return createScriptObject(value); 
}

template <>
void* IntType<uint16>::createScriptObject(IObject* object, const IProp* prop){
	uint16 value = object->getPropUint16(prop);
	return createScriptObject(value); 
}

template <>
void* IntType<int32>::createScriptObject(IObject* object, const IProp* prop){
	int32 value = object->getPropInt32(prop);
	return createScriptObject(value); 
}

template <>
void* IntType<int8>::createScriptObject(IArray* array, const int32 index){
	int8 value = array->getPropInt8(index);
	return createScriptObject(value); 
}

template <>
void* IntType<int16>::createScriptObject(IArray* array, const int32 index){
	int16 value = array->getPropInt16(index);
	return createScriptObject(value); 
}

template <>
void* IntType<uint8>::createScriptObject(IArray* array, const int32 index){
	uint8 value = array->getPropUint8(index);
	return createScriptObject(value); 
}

template <>
void* IntType<uint16>::createScriptObject(IArray* array, const int32 index){
	uint16 value = array->getPropUint16(index);
	return createScriptObject(value); 
}

template <>
void* IntType<int32>::createScriptObject(IArray* array, const int32 index){
	int32 value = array->getPropInt32(index);
	return createScriptObject(value); 
}

template <typename SPECIFY_TYPE>
void* IntType<SPECIFY_TYPE>::createScriptObject(SPECIFY_TYPE val){
	PyObject* pyVal = PyLong_FromLong(val);
	if(PyErr_Occurred()){
		PyErr_Clear();
		if(pyVal){
			Py_DECREF(pyVal);
			pyVal = NULL;
		}

		pyVal = PyLong_FromUnsignedLong(val);
		if(PyErr_Occurred()){
			PyErr_Clear();
			PyErr_Format(PyExc_TypeError, "IntType::createScriptObjectEx: errval=%d", val);
			PyErr_PrintEx(0);
			if(pyVal){
				Py_DECREF(pyVal);
				pyVal = NULL;
				return nullptr;
			}
		}
	}

	return pyVal;
}

template <typename SPECITY_TYPE>
void* IntType<SPECITY_TYPE>::parseDefaultStr(const char* defaultVal){
	SPECITY_TYPE val = sl::CStringUtils::StringAsInt32(defaultVal);
	return createScriptObject(val);
}

class UInt64Type: public PyDataType{
public:
	virtual ~UInt64Type(){}
	virtual bool addScriptObject(sl::IBStream& stream, void* value);
	virtual bool addScriptObject(IObject* object, const IProp* prop, void* value);
	virtual bool addScriptObject(IArray* array, const int32 index, void* value);
	virtual void* createScriptObject(const sl::OBStream& stream);
	virtual void* createScriptObject(IObject* object, const IProp* prop);
	virtual void* createScriptObject(IArray* array, const int32 index);
	virtual const char* getName() const {return "UINT64";}
	virtual const int32 getSize() const {return sizeof(uint64);}
	virtual const int8 getType() const {return DTYPE_UINT64;}
	virtual void* parseDefaultStr(const char* defaultValStr);

protected:
	bool addScriptObject(uint64& val, void* value);
	void* createScriptObject(uint64 value);
};

class UInt32Type: public PyDataType{
public:
	virtual ~UInt32Type() {}
	virtual bool addScriptObject(sl::IBStream& stream, void* value);
	virtual bool addScriptObject(IObject* object, const IProp* prop, void* value);
	virtual bool addScriptObject(IArray* array, const int32 index, void* value);
	virtual void* createScriptObject(const sl::OBStream& stream);
	virtual void* createScriptObject(IObject* object, const IProp* prop);
	virtual void* createScriptObject(IArray* array, const int32 index);
	virtual const char* getName() const {return "UINT32";}
	virtual const int32 getSize() const {return sizeof(uint32);}
	virtual const int8 getType() const {return DTYPE_UINT32;}
	virtual void* parseDefaultStr(const char* defaultValStr);

protected:
	bool addScriptObject(uint32& val, void* value);
	void* createScriptObject(uint32 value);
};

class Int64Type: public PyDataType{
public:
	virtual ~Int64Type() {}
	virtual bool addScriptObject(sl::IBStream& stream, void* value);
	virtual bool addScriptObject(IObject* object, const IProp* prop, void* value);
	virtual bool addScriptObject(IArray* array, const int32 index, void* value);
	virtual void* createScriptObject(const sl::OBStream& stream);
	virtual void* createScriptObject(IObject* object, const IProp* prop);
	virtual void* createScriptObject(IArray* array, const int32 index);
	virtual const char* getName() const {return "INT64";}
	virtual const int32 getSize() const {return sizeof(int64);}
	virtual const int8 getType() const {return DTYPE_INT64;}
	virtual void* parseDefaultStr(const char* defaultValStr);

protected:
	bool addScriptObject(int64& val, void* value);
	void* createScriptObject(int64 value);
};


class FloatType: public PyDataType{
public:
	virtual ~FloatType() {}
	virtual bool addScriptObject(sl::IBStream& stream, void* value);
	virtual bool addScriptObject(IObject* object, const IProp* prop, void* value);
	virtual bool addScriptObject(IArray* array, const int32 index, void* value);
	virtual void* createScriptObject(const sl::OBStream& stream);
	virtual void* createScriptObject(IObject* object, const IProp* prop);
	virtual void* createScriptObject(IArray* array, const int32 index);
	virtual const char* getName() const {return "FLOAT";} 
	virtual const int32 getSize() const {return sizeof(float);}
	virtual const int8 getType() const {return DTYPE_FLOAT;}
	virtual void* parseDefaultStr(const char* defaultValStr);

protected:
	bool addScriptObject(float& val, void* value);
	void* createScriptObject(float value);
};

class DoubleType: public PyDataType{
public:
	virtual ~DoubleType(){}
	virtual bool addScriptObject(sl::IBStream& stream, void* value);
	virtual bool addScriptObject(IObject* object, const IProp* prop, void* value);
	virtual bool addScriptObject(IArray* array, const int32 index, void* value);
	virtual void* createScriptObject(const sl::OBStream& stream);
	virtual void* createScriptObject(IObject* object, const IProp* prop);
	virtual void* createScriptObject(IArray* array, const int32 index);
	virtual const char* getName() const {return "DOUBLE";} 
	virtual const int32 getSize() const {return sizeof(double);}
	virtual const int8 getType() const {return DTYPE_DOUBLE;}
	virtual void* parseDefaultStr(const char* defaultValStr);

protected:
	bool addScriptObject(double& val, void* value);
	void* createScriptObject(double value);
};

class Vector2Type: public PyDataType{
public:
	virtual ~Vector2Type(){}
	virtual bool addScriptObject(sl::IBStream& stream, void* value);
	virtual bool addScriptObject(IObject* object, const IProp* prop, void* value);
	virtual bool addScriptObject(IArray* array, const int32 index, void* value);
	virtual void* createScriptObject(const sl::OBStream& stream);
	virtual void* createScriptObject(IObject* object, const IProp* prop);
	virtual void* createScriptObject(IArray* array, const int32 index);
	virtual const char* getName() const {return "VECTOR2";} 
	virtual const int32 getSize() const {return 2 * sizeof(float);}
	virtual const int8 getType() const {return DTYPE_BLOB;}
	virtual void* parseDefaultStr(const char* defaultValStr);

protected:
	bool addScriptObject(float* val, void* value);
	void* createScriptObject(float* val);
};

class Vector3Type: public PyDataType{
public:
	virtual ~Vector3Type(){}
	virtual bool addScriptObject(sl::IBStream& stream, void* value);
	virtual bool addScriptObject(IObject* object, const IProp* prop, void* value);
	virtual bool addScriptObject(IArray* array, const int32 index, void* value);
	virtual void* createScriptObject(const sl::OBStream& stream);
	virtual void* createScriptObject(IObject* object, const IProp* prop);
	virtual void* createScriptObject(IArray* array, const int32 index);
	virtual const char* getName() const {return "VECTOR3";}
	virtual const int32 getSize() const {return 3 * sizeof(float);}
	virtual const int8 getType() const {return DTYPE_BLOB;}
	virtual void* parseDefaultStr(const char* defaultValStr);

protected:
	bool addScriptObject(float* val, void* value);
	void* createScriptObject(float* val);
};

class Vector4Type: public PyDataType{
public:
	virtual ~Vector4Type(){}
	virtual bool addScriptObject(sl::IBStream& stream, void* value);
	virtual bool addScriptObject(IObject* object, const IProp* prop, void* value);
	virtual bool addScriptObject(IArray* array, const int32 index, void* value);
	virtual void* createScriptObject(const sl::OBStream& stream);
	virtual void* createScriptObject(IObject* object, const IProp* prop);
	virtual void* createScriptObject(IArray* array, const int32 index);
	virtual const char* getName() const {return "VECTOR4";}
	virtual const int32 getSize() const {return 4 * sizeof(float);}
	virtual const int8 getType() const {return DTYPE_BLOB;}
	virtual void* parseDefaultStr(const char* defaultValStr);

protected:
	bool addScriptObject(float* val, void* value);
	void* createScriptObject(float* val);
};

class StringType: public PyDataType{
public:
	virtual ~StringType(){}
	virtual bool addScriptObject(sl::IBStream& stream, void* value);
	virtual bool addScriptObject(IObject* object, const IProp* prop, void* value);
	virtual bool addScriptObject(IArray* array, const int32 index, void* value);
	virtual void* createScriptObject(const sl::OBStream& stream);
	virtual void* createScriptObject(IObject* object, const IProp* prop);
	virtual void* createScriptObject(IArray* array, const int32 index);
	virtual const char* getName() const {return "STRING";}
	virtual const int8 getType() const {return DTYPE_STRING;}
	virtual void* parseDefaultStr(const char* defaultValStr);

protected:
	char* addScriptObject(void* value);
	void* createScriptObject(const char* value);
};

class UnicodeType: public PyDataType{
public:
	virtual ~UnicodeType(){}
	virtual bool addScriptObject(sl::IBStream& stream, void* value);
	virtual bool addScriptObject(IObject* object, const IProp* prop, void* value);
	virtual bool addScriptObject(IArray* array, const int32 index, void* value);
	virtual void* createScriptObject(const sl::OBStream& stream);
	virtual void* createScriptObject(IObject* object, const IProp* prop);
	virtual void* createScriptObject(IArray* array, const int32 index);
	virtual const char* getName() const {return "UNICODE";}
	virtual const int8 getType() const {return DTYPE_BLOB;}
	virtual void* parseDefaultStr(const char* defaultValStr);

protected:
	void* createScriptObject(const void* data, const int32 size);
		
};

class BlobType: public PyDataType{
public:
	virtual ~BlobType(){}
	virtual bool addScriptObject(sl::IBStream& stream, void* value);
	virtual bool addScriptObject(IObject* object, const IProp* prop, void* value);
	virtual bool addScriptObject(IArray* array, const int32 index, void* value);
	virtual void* createScriptObject(const sl::OBStream& stream);
	virtual void* createScriptObject(IObject* object, const IProp* prop);
	virtual void* createScriptObject(IArray* array, const int32 index);
	virtual const char* getName() const {return "BLOB";}
	virtual const int8 getType() const {return DTYPE_BLOB;}
	virtual void* parseDefaultStr(const char* defaultValStr);

protected:
	void* createScriptObject(const void* data, const int32 size);
};

class PythonType: public PyDataType{
public:
	virtual ~PythonType(){}
	virtual bool addScriptObject(sl::IBStream& stream, void* value);
	virtual bool addScriptObject(IObject* object, const IProp* prop, void* value);
	virtual bool addScriptObject(IArray* array, const int32 index, void* value);
	virtual void* createScriptObject(const sl::OBStream& stream);
	virtual void* createScriptObject(IObject* object, const IProp* prop);
	virtual void* createScriptObject(IArray* array, const int32 index);
	virtual const char* getName() const {return "PYTHON";}
	virtual const int8 getType() const {return DTYPE_BLOB;}
	virtual void* parseDefaultStr(const char* defaultValStr);
};

class PyDictType : public PythonType{
public:
	virtual ~PyDictType(){}
	const char* getName() const { return "PY_DICT";}
	virtual void* parseDefaultStr(const char* defaultValStr);
};

class PyTupleType: public PythonType{
public:
	virtual ~PyTupleType() {}
	const char* getName() const { return "PY_TUPLE"; }
	virtual void* parseDefaultStr(const char* defaultValStr);
};

class PyListType: public PythonType{
public:
	virtual ~PyListType() {}
	const char* getName() const { return "PY_LIST"; }
	virtual void* parseDefaultStr(const char* defaultValStr);
};

class PyFixedArrayType: public PythonType{
public:
	virtual ~PyFixedArrayType() {}
	
	virtual bool addScriptObject(sl::IBStream& stream, void* value);
	virtual bool addScriptObject(IObject* object, const IProp* prop, void* value);
	virtual bool addScriptObject(IArray* array, const int32 index, void* value);
	virtual void* createScriptObject(const sl::OBStream& stream);
	virtual void* createScriptObject(IObject* object, const IProp* prop);
	virtual void* createScriptObject(IArray* array, const int32 index);
	virtual const char* getName() const {return "ARRAY";}
	virtual const int8 getType() const {return DTYPE_ARRAY;}
	virtual bool initType(const sl::ISLXmlNode* typeNode);
	virtual void addDataTypeInfo(sl::IBStream& stream);
	virtual void* parseDefaultStr(const char* defaultValStr);
	virtual PyObject* createNewItemFromObj(PyObject* pyobj);
//	virtual PyObject* createNewFromObj(PyObject* pyobj);
	virtual IDataType* arrayDataType() {return _dataType;}

protected:
	IDataType* _dataType;
};

class PyFixedDictType: public PythonType{
public:
	typedef std::vector<std::pair<string, IDataType*>> FIXEDDICT_KEYTYPE_MAP;
	PyFixedDictType():_pyImplObj(NULL), _pyCreateObjFromDict(NULL), _pyGetDictFromObj(NULL), _pyIsSameType(NULL){}
	virtual ~PyFixedDictType() {}
	const char* getName() const {return "FIXED_DICT";}
	virtual const int8 getType() const {return DTYPE_DICT;}
	virtual bool initType(const sl::ISLXmlNode* typeNode);
	bool loadImplModule(std::string moduleName);
	void addDataTypeInfo(sl::IBStream& stream);
	bool hasImpl() const { return _pyImplObj != NULL; }
	virtual PyObject* createNewItemFromObj(const char* keyName, PyObject* pyobj);
	virtual PyObject* createNewFromObj(PyObject* pyobj);

	virtual bool addScriptObject(sl::IBStream& stream, void* value);
	virtual bool addScriptObject(IObject* object, const IProp* prop, void* value){return true;}
	virtual bool addScriptObject(IArray* array, const int32 index, void* value){return true;}
	virtual void* createScriptObject(const sl::OBStream& stream);
	virtual void* createScriptObject(IObject* object, const IProp* prop){return NULL;}
	virtual void* createScriptObject(IArray* array, const int32 index){return NULL;}
	virtual void* parseDefaultStr(const char* defaultValStr);
	virtual std::vector<std::pair<string, IDataType*>> dictDataType() {return _dictItems;}

	PyObject* implCreateObjFromDict(PyObject* dictData);
	PyObject* implGetDictFromObj(PyObject* pyObj);

	inline FIXEDDICT_KEYTYPE_MAP& getKeyTypes() {return _dictItems;}

protected:
	FIXEDDICT_KEYTYPE_MAP _dictItems;
	PyObject* _pyImplObj;
	PyObject* _pyCreateObjFromDict;
	PyObject* _pyGetDictFromObj;
	PyObject* _pyIsSameType;
	std::string _moduleName;
};

class MailBoxType: public PyDataType{
public:
	virtual ~MailBoxType(){}
	virtual bool addScriptObject(sl::IBStream& stream, void* value);
	virtual bool addScriptObject(IObject* object, const IProp* prop, void* value){return true;}
	virtual bool addScriptObject(IArray* array, const int32 index, void* value) {return true;}
	virtual void* createScriptObject(const sl::OBStream& stream);
	virtual void* createScriptObject(IObject* object, const IProp* prop) {return NULL;}
	virtual void* createScriptObject(IArray* array, const int32 index){return NULL;}
	virtual const int32 getSize() const;
	virtual const char* getName() const {return "MAILBOX";}
	virtual const int8 getType() const {return DTYPE_BLOB;}
	virtual void* parseDefaultStr(const char* defaultValStr){ Py_RETURN_NONE;}
};



class MailBox: public PyDataType{
public:
	MailBox();
	virtual ~MailBox(){}
protected:

}; 
#endif
