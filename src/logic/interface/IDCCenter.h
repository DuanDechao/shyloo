#ifndef SL_IDCCENTER_H
#define SL_IDCCENTER_H
#include "slmulti_sys.h"
#include "slimodule.h"
#include "slstring.h"
#include "GameDefine.h"
#include "Attr.h"
#include <vector>
#define MAX_PROP_NAME_LEN 64
#define MAX_OBJECT_NAME_LEN 64
#define MAX_METHOD_NAME_LEN 128
enum{
	DTYPE_INT8 = 1,
	DTYPE_INT16,
	DTYPE_INT32,
	DTYPE_INT64,

	DTYPE_UINT8,
	DTYPE_UINT16,
	DTYPE_UINT32,
	DTYPE_UINT64,

	DTYPE_STRING,

	DTYPE_CANT_BE_KEY,
	DTYPE_FLOAT = DTYPE_CANT_BE_KEY,
	DTYPE_DOUBLE,
	DTYPE_STRUCT,
	DTYPE_BLOB,
	DTYPE_DICT,
	DTYPE_ARRAY
};

class IObject;
class ISubProp;
class IProp{
public:
	virtual const int32 getName() const = 0;
	virtual const char* getNameString() const = 0;
	virtual const int8 getType(IObject* object) const = 0;
	virtual const int32 getSetting(IObject* object) const = 0;
	virtual const int32 getIndex(IObject* object) const = 0;
    virtual const void* getExtra(IObject* object) const = 0;
    virtual const int32 getSize(IObject* object) const = 0;
	virtual const char* getDefaultVal(IObject* object) const = 0;
	virtual const int8 getType(const char* objectType) const = 0;
	virtual const int32 getSetting(const char* objectType) const = 0;
	virtual const int32 getIndex(const char* objectType) const = 0;
    virtual const void* getExtra(const char* objectType) const = 0;
	virtual const int32 getSize(const char* objectType) const = 0;
	virtual const char* getDefaultVal(const char* objectType) const = 0;
    virtual const int8 getType(const int32 objTypeId) const = 0;
    virtual const int32 getSetting(const int32 objTypeId) const = 0;
    virtual const int32 getIndex(const int32 objTypeId) const = 0;
    virtual const void* getExtra(const int32 objTypeId) const = 0;
    virtual const int32 getSize(const int32 objTypeId) const = 0;
	virtual const char* getDefaultVal(const int32 objTypeId) const = 0;

	virtual ISubProp* addDictProp(const int32 objTypeId, const char* elePropName, const int32 type, const int32 size) = 0;
	virtual ISubProp* addDictProp(const char* objectType, const char* elePropName, const int32 type, const int32 size) = 0;
	virtual ISubProp* addArrayProp(const int32 objTypeId, const int32 type, const int32 size) = 0;
	virtual ISubProp* addArrayProp(const char* objectType, const int32 type, const int32 size) = 0;
};

class ISubProp{
public:
	virtual const int8 getType() const = 0;
	virtual ISubProp* addDictProp(const char* elePropName, const int32 type, const int32 size) = 0;
	virtual ISubProp* addArrayProp(const int32 type, const int32 size) = 0;
};

class IRow{
public:
	virtual ~IRow() {}
	virtual int32 getRowIndex() const = 0;

	virtual int8 getDataInt8(const int32 col) const = 0;
	virtual int16 getDataInt16(const int32 col) const = 0;
	virtual int32 getDataInt32(const int32 col) const = 0;
	virtual int64 getDataInt64(const int32 col) const = 0;
	virtual float getDataFloat(const int32 col) const = 0;
	virtual const char* getDataString(const int32 col) const = 0;
	virtual const void* getDataBlob(const int32 col, int32& size) const = 0;

	virtual void setDataInt8(const int32 col, const int8 value) const = 0;
	virtual void setDataInt16(const int32 col, const int16 value) const = 0;
	virtual void setDataInt32(const int32 col, const int32 value) const = 0;
	virtual void setDataInt64(const int32 col, const int64 value) const = 0;
	virtual void setDataFloat(const int32 col, const float value) const = 0;
	virtual void setDataString(const int32 col, const char * value) const = 0;
	virtual void setDataBlob(const int32 col, const void* data, const int32 size) const = 0;
};


class ITableControl{
public:
	virtual IObject* getHost() const = 0;
	virtual int32 rowCount() const = 0;

	virtual void clearRows() = 0;
	virtual const IRow* findRow(const int64 key) const = 0;
	virtual const IRow* findRow(const char* key) const = 0;
	virtual const IRow* getRow(const int32 index) const = 0;
	virtual bool delRow(const int32 index) = 0;

	virtual IRow* addRow() = 0;
	virtual IRow* addRowKeyInt8(const int8 key) = 0;
	virtual IRow* addRowKeyInt16(const int16 key) = 0;
	virtual IRow* addRowKeyInt32(const int32 key) = 0;
	virtual IRow* addRowKeyInt64(const int64 key) = 0;
	virtual IRow* addRowKeyString(const char* key) = 0;
	virtual bool swapRowIndex(const int32 src, const int32 dst) = 0;
};

#define DEL_TABLE_ROW(table, row) { \
	SLASSERT(table && row, "table or row index is not invailed"); \
	if (table && row) {	\
		int32 lastRow = table->rowCount() - 1; \
		if (lastRow != row->getRowIndex()){	\
			table->swapRowIndex(row->getRowIndex(), lastRow); \
		}	\
		table->delRow(lastRow); \
	}\
}

#define ANY_CALL nullptr
typedef std::function<void(sl::api::IKernel *, IObject *, const char *, const IProp *, const bool)> PropCallBack;
#define RGS_PROP_CHANGER(obj, prop, cb) obj->rgsPropChangeCB(prop, std::bind(&cb, this, std::placeholders::_1, std::placeholders::_2,  std::placeholders::_3, std::placeholders::_4, std::placeholders::_5), #cb)

class IArray;
class IDict{
public:
	virtual ~IDict() {}

	virtual bool setPropInt8(const char* name, const int8 data) = 0;
	virtual bool setPropInt16(const char* name, const int16 data) = 0;
	virtual bool setPropInt32(const char* name, const int32 data) = 0;
	virtual bool setPropInt64(const char* name, const int64 data) = 0;

	virtual bool setPropUint8(const char* name, const uint8 data) = 0;
	virtual bool setPropUint16(const char* name, const uint16 data) = 0;
	virtual bool setPropUint32(const char* name, const uint32 data) = 0;
	virtual bool setPropUint64(const char* name, const uint64 data) = 0;
	
	virtual bool setPropFloat(const char* name, const float data) = 0;
	virtual bool setPropDouble(const char* name, const double data) = 0;
	virtual bool setPropString(const char* name, const char* data) = 0;
	virtual bool setPropStruct(const char* name, const void* data, const int32 size) = 0;
	virtual bool setPropBlob(const char* name, const void* data, const int32 size) = 0;

	virtual int8 getPropInt8(const char* name) const = 0;
	virtual int16 getPropInt16(const char* name) const = 0;
	virtual int32 getPropInt32(const char* name) const = 0;
	virtual int64 getPropInt64(const char* name) const = 0;

	virtual uint8 getPropUint8(const char* name) const = 0;
	virtual uint16 getPropUint16(const char* name) const = 0;
	virtual uint32 getPropUint32(const char* name) const = 0;
	virtual uint64 getPropUint64(const char* name) const = 0;

	virtual float getPropFloat(const char* name) const = 0;
	virtual double getPropDouble(const char* name) const = 0;
	virtual const char* getPropString(const char* name) const = 0;
	virtual const void* getPropStruct(const char* name, int32& size) const = 0;
	virtual const void* getPropBlob(const char* name, int32& size) const = 0;
	virtual const void* getPropData(const char* name, int32& size) const = 0;

	virtual IDict* getPropDict(const char* name) const = 0;
	virtual IArray* getPropArray(const char* name) const = 0;
};

class IArray{
public:
	virtual ~IArray() {}

	virtual int32 size() = 0;
	virtual void remove(const int32 index) = 0;
	virtual bool setPropInt8(const int32 index, const int8 data) = 0;
	virtual bool setPropInt16(const int32 index, const int16 data) = 0;
	virtual bool setPropInt32(const int32 index, const int32 data) = 0;
	virtual bool setPropInt64(const int32 index, const int64 data) = 0;

	virtual bool setPropUint8(const int32 index, const uint8 data) = 0;
	virtual bool setPropUint16(const int32 index, const uint16 data) = 0;
	virtual bool setPropUint32(const int32 index, const uint32 data) = 0;
	virtual bool setPropUint64(const int32 index, const uint64 data) = 0;
	
	virtual bool setPropFloat(const int32 index, const float data) = 0;
	virtual bool setPropDouble(const int32 index, const double data) = 0;
	virtual bool setPropString(const int32 index, const char* data) = 0;
	virtual bool setPropStruct(const int32 index, const void* data, const int32 size) = 0;
	virtual bool setPropBlob(const int32 index, const void* data, const int32 size) = 0;

	virtual int8 getPropInt8(const int32 index) const = 0;
	virtual int16 getPropInt16(const int32 index) const = 0;
	virtual int32 getPropInt32(const int32 index) const = 0;
	virtual int64 getPropInt64(const int32 index) const = 0;

	virtual uint8 getPropUint8(const int32 index) const = 0;
	virtual uint16 getPropUint16(const int32 index) const = 0;
	virtual uint32 getPropUint32(const int32 index) const = 0;
	virtual uint64 getPropUint64(const int32 index) const = 0;

	virtual float getPropFloat(const int32 index) const = 0;
	virtual double getPropDouble(const int32 index) const = 0;
	virtual const char* getPropString(const int32 index) const = 0;
	virtual const void* getPropStruct(const int32 index, int32& size) const = 0;
	virtual const void* getPropBlob(const int32 index, int32& size) const = 0;
	virtual const void* getPropData(const int32 index, int32& size) const = 0;

	virtual IDict* getPropDict(const int32 index) const = 0;
	virtual IArray* getPropArray(const int32 index) const = 0;
};

class IObject{
public:
    virtual ~IObject() {}
	virtual const uint64 getID() const = 0;

	virtual const std::vector<const IProp*>& getObjProps(bool noParent = false) const = 0;
	virtual const char* getObjTypeString() const = 0;
    virtual const int32 getObjectType() const = 0;
	virtual bool isShadow() const = 0;

	virtual bool setPropInt8(const IProp* prop, const int8 data, const bool sync = true) = 0;
	virtual bool setPropInt16(const IProp* prop, const int16 data, const bool sync = true) = 0;
	virtual bool setPropInt32(const IProp* prop, const int32 data, const bool sync = true) = 0;
	virtual bool setPropInt64(const IProp* prop, const int64 data, const bool sync = true) = 0;
	virtual bool setPropUint8(const IProp* prop, const uint8 data, const bool sync = true) = 0;
	virtual bool setPropUint16(const IProp* prop, const uint16 data, const bool sync = true) = 0;
	virtual bool setPropUint32(const IProp* prop, const uint32 data, const bool sync = true) = 0;
	virtual bool setPropUint64(const IProp* prop, const uint64 data, const bool sync = true) = 0;

	virtual bool setPropFloat(const IProp* prop, const float data, const bool sync = true) = 0;
	virtual bool setPropDouble(const IProp* prop, const double data, const bool sync = true) = 0;
	virtual bool setPropString(const IProp* prop, const char* data, const bool sync = true) = 0;
	virtual bool setPropStruct(const IProp* prop, const void* data, const int32 size, const bool sync = true) = 0;
	virtual bool setPropBlob(const IProp* prop, const void* data, const int32 size, const bool sync = true) = 0;

	virtual bool setTempInt8(const IProp* prop, const int8 data) = 0;
	virtual bool setTempInt16(const IProp* prop, const int16 data) = 0;
	virtual bool setTempInt32(const IProp* prop, const int32 data) = 0;
	virtual bool setTempInt64(const IProp* prop, const int64 data) = 0;
	virtual bool setTempFloat(const IProp* prop, const float data) = 0;
	virtual bool setTempDouble(const IProp* prop, const double data) = 0;
	virtual bool setTempString(const IProp* prop, const char* data) = 0;
	virtual bool setTempStruct(const IProp* prop, const void* data, const int32 size) = 0;
	virtual bool setTempBlob(const IProp* prop, const void* data, const int32 size) = 0;

	virtual int8 getPropInt8(const IProp* prop) const = 0;
	virtual int16 getPropInt16(const IProp* prop) const = 0;
	virtual int32 getPropInt32(const IProp* prop) const = 0;
	virtual int64 getPropInt64(const IProp* prop) const = 0;

	virtual uint8 getPropUint8(const IProp* prop) const = 0;
	virtual uint16 getPropUint16(const IProp* prop) const = 0;
	virtual uint32 getPropUint32(const IProp* prop) const = 0;
	virtual uint64 getPropUint64(const IProp* prop) const = 0;

	virtual float getPropFloat(const IProp* prop) const = 0;
	virtual double getPropDouble(const IProp* prop) const = 0;
	virtual const char* getPropString(const IProp* prop) const = 0;
	virtual const void* getPropStruct(const IProp* prop, int32& size) const = 0;
	virtual const void* getPropBlob(const IProp* prop, int32& size) const = 0;
	virtual const void* getPropData(const IProp* prop, int32& size) const = 0;
	virtual IDict* getPropDict(const IProp* prop) const = 0;
	virtual IArray* getPropArray(const IProp* prop) const = 0;

	virtual int8 getTempInt8(const IProp* prop) const = 0;
	virtual int16 getTempInt16(const IProp* prop) const = 0;
	virtual int32 getTempInt32(const IProp* prop) const = 0;
	virtual int64 getTempInt64(const IProp* prop) const = 0;
	virtual float getTempFloat(const IProp* prop) const = 0;
	virtual double getTempDouble(const IProp* prop) const = 0;
	virtual const char* getTempString(const IProp* prop) const = 0;
	virtual const void* getTempStruct(const IProp* prop, int32& size) const = 0;
	virtual const void* getTempBlob(const IProp* prop, int32& size) const = 0;

	virtual ITableControl* findTable(const int32 name) const = 0;

	virtual bool rgsPropChangeCB(const IProp* prop, const PropCallBack& cb, const char* info) = 0;
};

class IObjectMgr : public sl::api::IModule{
public:
	virtual ~IObjectMgr() {}

	virtual const IProp* getPropByName(const char* name) const = 0;
	virtual const IProp* getTempPropByName(const char* name) const = 0;
	virtual const IProp* getPropByNameId(const int32 name) const = 0;
	virtual IObject* create(const char* file, const int32 line, const char* name, bool isShadow = false) = 0;
	virtual IObject* createById(const char* file, const int32 line, const char* name, const uint64 id, bool isShadow = false) = 0;
	virtual void recover(IObject* object) = 0;
	virtual IObject* findObject(const uint64 id) = 0;
	virtual ITableControl* createStaticTable(const char* name, const char* model, const char* file, const int32 line) = 0;
	virtual const IProp* appendObjectProp(const char* objectName, const char* propName, const int8 type, const int32 size, const int32 setting, const int32 index, const void* extra, const char* defaultVal = "") = 0;
	virtual const IProp* appendObjectTempProp(const char* objectName, const char* propName, const int8 type, const int32 size, const int32 setting, const int32 index, const void* extra, const char* defaultVal = "") = 0;
	virtual void setObjectTypeSize(const int32 size) = 0;
    virtual const int32 getObjectType(const char* objectName) = 0;
	virtual bool appendTableColumnInfo(const char* tableName, const int16 type, const int32 typeSize, bool isKey) = 0;
};

#define CREATE_OBJECT(mgr, ...) mgr->create(__FILE__, __LINE__, __VA_ARGS__)
#define CREATE_OBJECT_BYID(mgr, ...) mgr->createById(__FILE__, __LINE__, __VA_ARGS__)
#define CREATE_STATIC_TABLE(mgr, name, model) mgr->createStaticTable(name, model, __FILE__, __LINE__)
#endif
