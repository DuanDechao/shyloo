#ifndef SL_LOGIC_ENTITY_DEF_DATA_TYPE_MGR_H
#define SL_LOGIC_ENTITY_DEF_DATA_TYPE_MGR_H
#include "slmulti_sys.h"
#include "slscript_object.h"
#include "DataType.h"
#include "IDCCenter.h"
class IObject;
class IProp;
class DataTypeMgr{
public:
	enum ScriptDataType{
		SDTYPE_INT8 = DTYPE_INT8,
		SDTYPE_INT16 = DTYPE_INT16,
		SDTYPE_INT32 = DTYPE_INT32,
		SDTYPE_INT64 = DTYPE_INT64,
		SDTYPE_UINT8 = DTYPE_UINT8,
		SDTYPE_UINT16 = DTYPE_UINT16,
		SDTYPE_UINT32 = DTYPE_UINT32,
		SDTYPE_UINT64 = DTYPE_UINT64,
		SDTYPE_STRING = DTYPE_STRUCT,
		SDTYPE_BLOB = DTYPE_BLOB,

		SDTYPE_OWN_TYPE = 100,
		SDTYPE_PYTHON,
	};

	typedef std::unordered_map<std::string, DataType*> DATATYPE_MAP;
	typedef std::unordered_map<DATATYPE_UID, DataType*> UID_DATATYPE_MAP;
	typedef std::unordered_map<std::string, DATATYPE_UID> TYPE_UID_MAP;

	DataTypeMgr();
	virtual ~DataTypeMgr();

	static bool initialize(const char* aliasFile);

	static bool addDataType(const char* name, DataType* dataType);
	static bool addDataType(DATATYPE_UID uid, DataType* dataType);
	static void delDataType(const char* name);

	static DataType* getDataType(const char* name);
	static DataType* getDataType(DATATYPE_UID uid);
	static DATATYPE_UID getDataTypeUid(const char* name);

	static PyObject* getPyAttrValue(IObject* object, const IProp* prop);
	static int32 setPyAttrValue(IObject* object, const IProp* prop, PyObject* value);
    static bool isDataType(PyObject* args, int8 type){return true;}

private:
	static DATATYPE_MAP _dataTypes;
	static UID_DATATYPE_MAP _uid_dataTypes;
	static TYPE_UID_MAP _typeUidMap;
};
#endif
