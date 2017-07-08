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
enum{
	DTYPE_INT8 = 0,
	DTYPE_INT16,
	DTYPE_INT32,
	DTYPE_INT64,
	DTYPE_STRING,

	DTYPE_CANT_BE_KEY,
	DTYPE_FLOAT = DTYPE_CANT_BE_KEY,
	DTYPE_STRUCT,
	DTYPE_BLOB
};

class IObject;
class IProp{
public:
	virtual const int32 getName() const = 0;
	virtual const int8 getType(IObject* object) const = 0;
	virtual const int32 getSetting(IObject* object) const = 0;
	virtual const int32 getIndex(IObject* object) const = 0;
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
	virtual const char * getDataString(const int32 col) const = 0;

	virtual void setDataInt8(const int32 col, const int8 value) const = 0;
	virtual void setDataInt16(const int32 col, const int16 value) const = 0;
	virtual void setDataInt32(const int32 col, const int32 value) const = 0;
	virtual void setDataInt64(const int32 col, const int64 value) const = 0;
	virtual void setDataFloat(const int32 col, const float value) const = 0;
	virtual void setDataString(const int32 col, const char * value) const = 0;
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

class IObject{
public:
    virtual ~IObject() {}
	virtual const int64 getID() const = 0;

	virtual const std::vector<const IProp*>& getObjProps(bool noParent = false) const = 0;
	virtual const char* getObjTypeString() const = 0;
	virtual bool isShadow() const = 0;

	virtual bool setPropInt8(const IProp* prop, const int8 data, const bool sync = true) = 0;
	virtual bool setPropInt16(const IProp* prop, const int16 data, const bool sync = true) = 0;
	virtual bool setPropInt32(const IProp* prop, const int32 data, const bool sync = true) = 0;
	virtual bool setPropInt64(const IProp* prop, const int64 data, const bool sync = true) = 0;
	virtual bool setPropFloat(const IProp* prop, const float data, const bool sync = true) = 0;
	virtual bool setPropString(const IProp* prop, const char* data, const bool sync = true) = 0;
	virtual bool setPropStruct(const IProp* prop, const void* data, const int32 size, const bool sync = true) = 0;
	virtual bool setPropBlob(const IProp* prop, const void* data, const int32 size, const bool sync = true) = 0;

	virtual bool setTempInt8(const IProp* prop, const int8 data) = 0;
	virtual bool setTempInt16(const IProp* prop, const int16 data) = 0;
	virtual bool setTempInt32(const IProp* prop, const int32 data) = 0;
	virtual bool setTempInt64(const IProp* prop, const int64 data) = 0;
	virtual bool setTempFloat(const IProp* prop, const float data) = 0;
	virtual bool setTempString(const IProp* prop, const char* data) = 0;
	virtual bool setTempStruct(const IProp* prop, const void* data, const int32 size) = 0;
	virtual bool setTempBlob(const IProp* prop, const void* data, const int32 size) = 0;

	virtual int8 getPropInt8(const IProp* prop) const = 0;
	virtual int16 getPropInt16(const IProp* prop) const = 0;
	virtual int32 getPropInt32(const IProp* prop) const = 0;
	virtual int64 getPropInt64(const IProp* prop) const = 0;
	virtual float getPropFloat(const IProp* prop) const = 0;
	virtual const char* getPropString(const IProp* prop) const = 0;
	virtual const void* getPropStruct(const IProp* prop, int32& size) const = 0;
	virtual const void* getPropBlob(const IProp* prop, int32& size) const = 0;

	virtual int8 getTempInt8(const IProp* prop) const = 0;
	virtual int16 getTempInt16(const IProp* prop) const = 0;
	virtual int32 getTempInt32(const IProp* prop) const = 0;
	virtual int64 getTempInt64(const IProp* prop) const = 0;
	virtual float getTempFloat(const IProp* prop) const = 0;
	virtual const char* getTempString(const IProp* prop) const = 0;
	virtual const void* getTempStruct(const IProp* prop, int32& size) const = 0;
	virtual const void* getTempBlob(const IProp* prop, int32& size) const = 0;

	virtual ITableControl* findTable(const int32 name) const = 0;

	virtual bool rgsPropChangeCB(const IProp* prop, const PropCallBack& cb, const char* info) = 0;
};


class IObjectMgr : public sl::api::IModule
{
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
};

#define CREATE_OBJECT(mgr, ...) mgr->create(__FILE__, __LINE__, __VA_ARGS__)
#define CREATE_OBJECT_BYID(mgr, ...) mgr->createById(__FILE__, __LINE__, __VA_ARGS__)
#define CREATE_STATIC_TABLE(mgr, name, model) mgr->createStaticTable(name, model, __FILE__, __LINE__)
#endif
