#ifndef SL_MMOBJECT_H
#define SL_MMOBJECT_H
#include "IDCCenter.h"
#include "ObjectStruct.h"

class OMemory;
class ObjectFSM;
class MMObject : public IObject{
public:
	MMObject(const char* name, const ObjectPropInfo* pPropInfo);
	~MMObject();
		
	virtual const char* getObjTypeString() const { return _name.c_str(); }
	inline const ObjectPropInfo* getObjectPropInfo() const { return _poPropInfo; }
	
	virtual const uint64 getID() const { return _objectId; }
	void setID(uint64 id){ _objectId = id; }

	virtual const std::vector<const IProp*>& getObjProps(bool noParent) const;

	virtual bool setPropInt8(const IProp* prop, const int8 data){ return setData(prop, DTYPE_INT8, &data, sizeof(int8)); }
	virtual bool setPropInt16(const IProp* prop, const int16 data){ return setData(prop, DTYPE_INT16, &data, sizeof(int16)); }
	virtual bool setPropInt32(const IProp* prop, const int32 data){ return setData(prop, DTYPE_INT32, &data, sizeof(int32)); }
	virtual bool setPropInt64(const IProp* prop, const int64 data){ return setData(prop, DTYPE_INT64, &data, sizeof(int64)); }
	virtual bool setPropFloat(const IProp* prop, const float data){ return setData(prop, DTYPE_FLOAT, &data, sizeof(float)); }
	virtual bool setPropString(const IProp* prop, const char* data){ return setData(prop, DTYPE_INT8, data, strlen(data)+1); }
	virtual bool setData(const IProp* prop, const int8 type, const void* data, const int32 size);

	virtual int8 getPropInt8(const IProp* prop) const { int32 size = sizeof(int8); return *(int8*)getData(prop, DTYPE_INT8, size); }
	virtual int16 getPropInt16(const IProp* prop) const { int32 size = sizeof(int16); return *(int16*)getData(prop, DTYPE_INT16, size); }
	virtual int32 getPropInt32(const IProp* prop) const { int32 size = sizeof(int32); return *(int32*)getData(prop, DTYPE_INT32, size); }
	virtual int64 getPropInt64(const IProp* prop) const { int32 size = sizeof(int64); return *(int64*)getData(prop, DTYPE_INT64, size); }
	virtual float getPropFloat(const IProp* prop) const { int32 size = sizeof(float); return *(float*)getData(prop, DTYPE_FLOAT, size); }
	virtual const char* getPropString(const IProp* prop) const { int32 size = 0; return (const char*)getData(prop, DTYPE_STRING, size); }
	virtual const void* getData(const IProp* prop, const int8 type, int32& size) const;

	ITableControl* findTable(const int32 name) const;

private:
	const sl::SLString<MAX_OBJECT_NAME_LEN>		_name;
	uint64										_objectId;
	const ObjectPropInfo*						_poPropInfo;
	std::unordered_map<int32, TableControl*>	_tables;
	OMemory*									_memory;
	ObjectFSM*									_objectFSM;

};
#endif