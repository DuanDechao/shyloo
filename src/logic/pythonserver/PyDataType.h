#ifndef SL_LOGIC_PY_DATA_TYPE_H
#define SL_LOGIC_PY_DATA_TYPE_H
#include "slmulti_sys.h"
#include "Python.h"
#include "IObjectDef.h"
#include "IDCCenter.h"
#include "slmemorystream.h"
#include "slxml_reader.h"
class PyDataType: public IDataType{
public:
	PyDataType()
		:_id(0), 
		_type(0),
		_aliasName("")
	{}

	virtual ~PyDataType(){}

	virtual bool initType(sl::ISLXmlNode* typeNode) {return true;}
	virtual void setUid(const uint16 id) {_id = id;}
	virtual const uint16 getUid() const {return _id;}
	virtual void setType(const int8 type) {_type = type;}
	virtual const int8 getType() const {return _type;}
	virtual void setAliasName(const char* aliasName) {_aliasName = aliasName;}
	virtual const char* getAliasName() const {return _aliasName.c_str();}
	virtual const int32 getSize() const {return 256;}	
	virtual void* createFromObject(IObject* object, const IProp* prop);
	virtual void addToObject(IObject* object, const IProp* prop, void* value);
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
	int8			_type;
	std::string		_aliasName;
	static std::unordered_map<std::string, IDataType*> s_pyDataTypes;
};

template <typename SPECIFY_TYPE>
class IntType: public PyDataType{
public:
	virtual ~IntType(){}
	virtual const char* getName() const {return "INT";}
	virtual const int32 getSize() const {return sizeof(int32);}
	virtual void addToStream(sl::IBStream& stream, void* value);
	virtual void* createFromStream(const sl::OBStream& stream);
	virtual void* createFromStreamEx(SPECIFY_TYPE val);
	virtual void* parseDefaultStr(const char* defaultValStr);

protected:
	SPECIFY_TYPE pyValueToInt(PyObject* pyValue);
};

template <>
inline const char* IntType<uint8>::getName()const {return "UINT8";}
template <>
inline const int32 IntType<uint8>::getSize()const {return sizeof(uint8);}

template <>
inline const char* IntType<uint16>::getName()const {return "UINT16";}
template <>
inline const int32 IntType<uint16>::getSize()const {return sizeof(uint16);}

template <>
inline const char* IntType<uint32>::getName() const {return "UINT32";}
template <>
inline const int32 IntType<uint32>::getSize()const {return sizeof(uint32);}

template <>
inline const char* IntType<int8>::getName() const {return "INT8";}
template <>
inline const int32 IntType<int8>::getSize()const {return sizeof(int8);}

template <>
inline const char* IntType<int16>::getName() const { return "INT16";}
template <>
inline const int32 IntType<int16>::getSize()const {return sizeof(int16);}

template <>
inline const char* IntType<int32>::getName() const {return "INT32";}
template <>
inline const int32 IntType<int32>::getSize()const {return sizeof(int32);}
template class IntType<int8>;
template class IntType<int16>;
template class IntType<uint8>;
template class IntType<uint16>;
template class IntType<int32>;

template <typename SPECIFY_TYPE>
void IntType<SPECIFY_TYPE>::addToStream(sl::IBStream& stream, void* value){
	PyObject* pyValue = (PyObject*)value;
	SPECIFY_TYPE v = (SPECIFY_TYPE)PyLong_AsLong(pyValue);
	if(PyErr_Occurred()){
		PyErr_Clear();

		v = (SPECIFY_TYPE)PyLong_AsUnsignedLong(pyValue);
		if(PyErr_Occurred()){
			PyErr_Clear();
			PyErr_Format(PyExc_TypeError, "IntType::pyValueToInt: pyValue(%s) is wrong!",
					(pyValue == NULL) ? "NULL" : pyValue->ob_type->tp_name);
			PyErr_PrintEx(0);

			v = 0;
		}
	}
	
	stream << v;
}

template <>
void* IntType<int8>::createFromStream(const sl::OBStream& stream){
	int8 value = 0;
	if(!stream.readInt8(value)){
		ECHO_ERROR("read Int8 failed from stream");
		return nullptr;
	}
	return createFromStreamEx(value); 
}

template <>
void* IntType<int16>::createFromStream(const sl::OBStream& stream){
	int16 value = 0;
	if(!stream.readInt16(value)){
		ECHO_ERROR("read Int8 failed from stream");
		return nullptr;
	}
	return createFromStreamEx(value); 
}

template <>
void* IntType<uint8>::createFromStream(const sl::OBStream& stream){
	uint8 value = 0;
	if(!stream.readUint8(value)){
		ECHO_ERROR("read Int8 failed from stream");
		return nullptr;
	}
	return createFromStreamEx(value); 
}

template <>
void* IntType<uint16>::createFromStream(const sl::OBStream& stream){
	uint16 value = 0;
	if(!stream.readUint16(value)){
		ECHO_ERROR("read Int8 failed from stream");
		return nullptr;
	}
	return createFromStreamEx(value); 
}

template <>
void* IntType<int32>::createFromStream(const sl::OBStream& stream){
	int32 value = 0;
	if(!stream.readInt32(value)){
		ECHO_ERROR("read Int8 failed from stream");
		return nullptr;
	}
	return createFromStreamEx(value); 
}

template <typename SPECIFY_TYPE>
void* IntType<SPECIFY_TYPE>::createFromStreamEx(SPECIFY_TYPE val){
	PyObject* pyVal = PyLong_FromLong(val);
	if(PyErr_Occurred()){
		PyErr_Clear();
		if(pyVal){
			Py_DECREF(pyVal);
			pyVal = NULL;
		}

		pyVal = PyLong_FromUnsignedLong(val);
		if(PyErr_Occurred()){
			PyErr_Format(PyExc_TypeError, "IntType::createFromStream: errval=%d", val);
			PyErr_PrintEx(0);
			if(pyVal){
				Py_DECREF(pyVal);
				pyVal = NULL;
				return PyLong_FromUnsignedLong(0);
			}
		}
	}

	return pyVal;
}

template <typename SPECITY_TYPE>
void* IntType<SPECITY_TYPE>::parseDefaultStr(const char* defaultVal){
	SPECITY_TYPE val = sl::CStringUtils::StringAsInt32(defaultVal);
	return createFromStreamEx(val);
}

class UInt64Type: public PyDataType{
public:
	virtual ~UInt64Type(){}
	virtual void addToStream(sl::IBStream& stream, void* value);
	virtual void* createFromStream(const sl::OBStream& stream);
	virtual const char* getName() const {return "UINT64";}
	virtual const int32 getSize() const {return sizeof(uint64);}
	virtual void* parseDefaultStr(const char* defaultValStr);

protected:
	uint64 pyValueToUint64(PyObject* pyValue);
};

class UInt32Type: public PyDataType{
public:
	virtual ~UInt32Type() {}
	virtual void addToStream(sl::IBStream& stream, void* value);
	virtual void* createFromStream(const sl::OBStream& stream);
	virtual const char* getName() const {return "UINT32";}
	virtual const int32 getSize() const {return sizeof(uint32);}
	virtual void* parseDefaultStr(const char* defaultValStr);
};

class Int64Type: public PyDataType{
public:
	virtual ~Int64Type() {}
	virtual void addToStream(sl::IBStream& stream, void* value);
	virtual void* createFromStream(const sl::OBStream& stream);
	virtual const char* getName() const {return "INT64";}
	virtual const int32 getSize() const {return sizeof(int64);}
	virtual void* parseDefaultStr(const char* defaultValStr);
};


class FloatType: public PyDataType{
public:
	virtual ~FloatType() {}
	virtual void addToStream(sl::IBStream& stream, void* value);
	virtual void* createFromStream(const sl::OBStream& stream);
	virtual const char* getName() const {return "FLOAT";} 
	virtual const int32 getSize() const {return sizeof(float);}
	virtual void* parseDefaultStr(const char* defaultValStr);
};

class DoubleType: public PyDataType{
public:
	virtual ~DoubleType(){}
	virtual void addToStream(sl::IBStream& stream, void* value);
	virtual void* createFromStream(const sl::OBStream& stream);
	virtual const char* getName() const {return "DOUBLE";} 
	virtual const int32 getSize() const {return sizeof(double);}
	virtual void* parseDefaultStr(const char* defaultValStr);
};

class Vector2Type: public PyDataType{
public:
	virtual ~Vector2Type(){}
	virtual void addToStream(sl::IBStream& stream, void* value);
	virtual void* createFromStream(const sl::OBStream& stream);
	virtual const char* getName() const {return "VECTOR2";} 
	virtual const int32 getSize() const {return 2 * sizeof(float);}
	virtual void* parseDefaultStr(const char* defaultValStr);
};

class Vector3Type: public PyDataType{
public:
	virtual ~Vector3Type(){}
	virtual void addToStream(sl::IBStream& stream, void* value);
	virtual void* createFromStream(const sl::OBStream& stream);
	virtual const char* getName() const {return "VECTOR3";}
	virtual const int32 getSize() const {return 3 * sizeof(float);}
	virtual void* parseDefaultStr(const char* defaultValStr);
};

class Vector4Type: public PyDataType{
public:
	virtual ~Vector4Type(){}
	virtual void addToStream(sl::IBStream& stream, void* value);
	virtual void* createFromStream(const sl::OBStream& stream);
	virtual const char* getName() const {return "VECTOR4";}
	virtual const int32 getSize() const {return 4 * sizeof(float);}
	virtual void* parseDefaultStr(const char* defaultValStr);
};

class StringType: public PyDataType{
public:
	virtual ~StringType(){}
	virtual void addToStream(sl::IBStream& stream, void* value);
	virtual void* createFromStream(const sl::OBStream& stream);
	virtual const char* getName() const {return "STRING";}
	virtual void* parseDefaultStr(const char* defaultValStr);
};

class UnicodeType: public PyDataType{
public:
	virtual ~UnicodeType(){}
	virtual void addToStream(sl::IBStream& stream, void* value);
	virtual void* createFromStream(const sl::OBStream& stream);
	virtual const char* getName() const {return "UNICODE";}
	virtual void* parseDefaultStr(const char* defaultValStr);
};

class BlobType: public PyDataType{
public:
	virtual ~BlobType(){}
	virtual void addToStream(sl::IBStream& stream, void* value);
	virtual void* createFromStream(const sl::OBStream& stream);
	virtual const char* getName() const {return "BLOB";}
	virtual void* parseDefaultStr(const char* defaultValStr);
};

class PythonType: public PyDataType{
public:
	virtual ~PythonType(){}
	virtual void addToStream(sl::IBStream& stream, void* value);
	virtual void* createFromStream(const sl::OBStream& stream);
	virtual const char* getName() const {return "PYTHON";}
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
	
	virtual void addToStream(sl::IBStream& stream, void* value);
	virtual void* createFromStream(const sl::OBStream& stream);
	virtual const char* getName() const {return "ARRAY";}
	virtual bool initType(const sl::ISLXmlNode* typeNode);
	virtual void addDataTypeInfo(sl::IBStream& stream);
	virtual void* parseDefaultStr(const char* defaultValStr);
	virtual PyObject* createNewItemFromObj(PyObject* pyobj);
	virtual PyObject* createNewFromObj(PyObject* pyobj);
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
	virtual bool initType(const sl::ISLXmlNode* typeNode);
	bool loadImplModule(std::string moduleName);
	void addDataTypeInfo(sl::IBStream& stream);
	bool hasImpl() const { return _pyImplObj != NULL; }
	virtual PyObject* createNewItemFromObj(const char* keyName, PyObject* pyobj);
	virtual PyObject* createNewFromObj(PyObject* pyobj);

	virtual void addToStream(sl::IBStream& stream, void* value);
	virtual void* createFromStream(const sl::OBStream& stream);
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
	virtual void addToStream(sl::IBStream& stream, void* value);
	virtual void* createFromStream(const sl::OBStream& stream);
	virtual const int32 getSize() const;
	virtual const char* getName() const {return "MAILBOX";}
	virtual void* parseDefaultStr(const char* defaultValStr){ Py_RETURN_NONE;}
};



class MailBox: public PyDataType{
public:
	MailBox();
	virtual ~MailBox(){}
protected:

}; 
#endif
