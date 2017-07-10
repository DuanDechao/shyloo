#ifndef SL_MMOBJECT_H
#define SL_MMOBJECT_H
#include "IDCCenter.h"
#include "ObjectStruct.h"
#include "slcallback.h"
#include "ObjectMgr.h"

class OMemory;
class ObjectFSM;
class MMObject : public IObject{
	typedef sl::CallBackType<const IProp*, PropCallBack>::type	 PROP_CB_POOL;
public:
	MMObject(const char* name, const ObjectPropInfo* pPropInfo);
    virtual ~MMObject();
		
	virtual const char* getObjTypeString() const { return _name.c_str(); }
	inline const ObjectPropInfo* getObjectPropInfo() const { return _poPropInfo; }
	
	virtual const int64 getID() const { return (int64)_objectId; }
	void setID(uint64 id){ _objectId = id; }

	virtual const std::vector<const IProp*>& getObjProps(bool noParent) const;

	virtual bool isShadow() const { return _isShadow; }

	void setShadow(const bool shadow) { _isShadow = shadow; }

	virtual bool setPropInt8(const IProp* prop, const int8 data, const bool sync){ return setData(prop, false, DTYPE_INT8, &data, sizeof(int8), sync); }
	virtual bool setPropInt16(const IProp* prop, const int16 data, const bool sync){ return setData(prop, false, DTYPE_INT16, &data, sizeof(int16), sync); }
	virtual bool setPropInt32(const IProp* prop, const int32 data, const bool sync){ return setData(prop, false, DTYPE_INT32, &data, sizeof(int32), sync); }
	virtual bool setPropInt64(const IProp* prop, const int64 data, const bool sync){ return setData(prop, false, DTYPE_INT64, &data, sizeof(int64), sync); }
	virtual bool setPropFloat(const IProp* prop, const float data, const bool sync){ return setData(prop, false, DTYPE_FLOAT, &data, sizeof(float), sync); }
	virtual bool setPropString(const IProp* prop, const char* data, const bool sync){
		if (data == nullptr)
			return setData(prop, false, DTYPE_STRING, "", (int32)strlen("") + 1, sync);
		return setData(prop, false, DTYPE_STRING, data, (int32)strlen(data) + 1, sync); 
	}
	virtual bool setPropStruct(const IProp* prop, const void* data, const int32 size, const bool sync) { return setData(prop, false, DTYPE_STRUCT, data, size, sync); }
	virtual bool setPropBlob(const IProp* prop, const void* data, const int32 size, const bool sync) { return setData(prop, false, DTYPE_BLOB, data, size, sync); }
	
	virtual bool setTempInt8(const IProp* prop, const int8 data){ return setData(prop, true, DTYPE_INT8, &data, sizeof(int8), false); }
	virtual bool setTempInt16(const IProp* prop, const int16 data){ return setData(prop, true, DTYPE_INT16, &data, sizeof(int16), false); }
	virtual bool setTempInt32(const IProp* prop, const int32 data){ return setData(prop, true, DTYPE_INT32, &data, sizeof(int32), false); }
	virtual bool setTempInt64(const IProp* prop, const int64 data){ return setData(prop, true, DTYPE_INT64, &data, sizeof(int64), false); }
	virtual bool setTempFloat(const IProp* prop, const float data){ return setData(prop, true, DTYPE_FLOAT, &data, sizeof(float), false); }
	virtual bool setTempString(const IProp* prop, const char* data){
		if (data == nullptr)
			return setData(prop, true, DTYPE_STRING, "", (int32)strlen("") + 1, false);
		return setData(prop, true, DTYPE_STRING, data, (int32)strlen(data) + 1, false); 
	}
	virtual bool setTempStruct(const IProp* prop, const void* data, const int32 size) { return setData(prop, true, DTYPE_STRUCT, data, size, false); }
	virtual bool setTempBlob(const IProp* prop, const void* data, const int32 size) { return setData(prop, true, DTYPE_BLOB, data, size, false); }

	virtual bool setData(const IProp* prop, const bool temp, const int8 type, const void* data, const int32 size, const bool sync);

	virtual int8 getPropInt8(const IProp* prop) const { int32 size = sizeof(int8); return *(int8*)getData(prop, false, DTYPE_INT8, size); }
	virtual int16 getPropInt16(const IProp* prop) const { int32 size = sizeof(int16); return *(int16*)getData(prop, false, DTYPE_INT16, size); }
	virtual int32 getPropInt32(const IProp* prop) const { int32 size = sizeof(int32); return *(int32*)getData(prop, false, DTYPE_INT32, size); }
	virtual int64 getPropInt64(const IProp* prop) const { int32 size = sizeof(int64); return *(int64*)getData(prop, false, DTYPE_INT64, size); }
	virtual float getPropFloat(const IProp* prop) const { int32 size = sizeof(float); return *(float*)getData(prop, false, DTYPE_FLOAT, size); }
	virtual const char* getPropString(const IProp* prop) const { int32 size = 0; return (const char*)getData(prop, false, DTYPE_STRING, size); }
	virtual const void* getPropStruct(const IProp* prop, int32& size) const {size = 0; return getData(prop, false, DTYPE_STRUCT, size); }
	virtual const void* getPropBlob(const IProp* prop, int32& size) const { size = 0; return getData(prop, false, DTYPE_BLOB, size); }

	virtual int8 getTempInt8(const IProp* prop) const { int32 size = sizeof(int8); return *(int8*)getData(prop, true, DTYPE_INT8, size); }
	virtual int16 getTempInt16(const IProp* prop) const { int32 size = sizeof(int16); return *(int16*)getData(prop, true, DTYPE_INT16, size); }
	virtual int32 getTempInt32(const IProp* prop) const { int32 size = sizeof(int32); return *(int32*)getData(prop, true, DTYPE_INT32, size); }
	virtual int64 getTempInt64(const IProp* prop) const { int32 size = sizeof(int64); return *(int64*)getData(prop, true, DTYPE_INT64, size); }
	virtual float getTempFloat(const IProp* prop) const { int32 size = sizeof(float); return *(float*)getData(prop, true, DTYPE_FLOAT, size); }
	virtual const char* getTempString(const IProp* prop) const { int32 size = 0; return (const char*)getData(prop, true, DTYPE_STRING, size); }
	virtual const void* getTempStruct(const IProp* prop, int32& size) const { size = 0; return getData(prop, true, DTYPE_STRUCT, size); }
	virtual const void* getTempBlob(const IProp* prop, int32& size) const { size = 0; return getData(prop, true, DTYPE_BLOB, size); }

	virtual const void* getData(const IProp* prop, const bool temp, const int8 type, int32& size) const;

	virtual ITableControl* findTable(const int32 name) const;

	virtual bool rgsPropChangeCB(const IProp* prop, const PropCallBack& cb, const char* info) { _propCBPool.Register(prop, cb, info); return true; }

private:
	inline void propCall(const IProp* prop, const bool sync){
		_propCBPool.Call(prop, ObjectMgr::getInstance()->getKernel(), this, _name.c_str(), prop, sync);
		_propCBPool.Call(nullptr, ObjectMgr::getInstance()->getKernel(), this, _name.c_str(), prop, sync);
	}

private:
	const sl::SLString<MAX_OBJECT_NAME_LEN>		_name;
	uint64										_objectId;
	bool										_isShadow;
	const ObjectPropInfo*						_poPropInfo;
	std::unordered_map<int32, TableControl*>	_tables;
	OMemory*									_memory;
	ObjectFSM*									_objectFSM;
	PROP_CB_POOL								_propCBPool;
	

};
#endif
