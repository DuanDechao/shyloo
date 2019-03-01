#ifndef SL_CORE_DATA_DICT_H
#define SL_CORE_DATA_DICT_H
#include "MMObject.h"
class DataDict: public IDict{
public:
	DataDict(const MMObject* object, const std::unordered_map<std::string, PropLayout*>& dictLayouts, const void* data, const int32 dataSize)
		:_dictLayouts(dictLayouts),
		 _object(object)
	{
		_memory = NEW OMemory((char*)data, dataSize);
	}

	virtual ~DataDict(){}
	
	virtual bool setPropInt8(const char* name, const int8 data){ return setData(name, DTYPE_INT8, &data, sizeof(int8)); }
	virtual bool setPropInt16(const char* name, const int16 data){ return setData(name, DTYPE_INT16, &data, sizeof(int16)); }
	virtual bool setPropInt32(const char* name, const int32 data){ return setData(name, DTYPE_INT32, &data, sizeof(int32)); }
	virtual bool setPropInt64(const char* name, const int64 data){ return setData(name, DTYPE_INT64, &data, sizeof(int64)); }

	virtual bool setPropUint8(const char* name, const uint8 data){ return setData(name, DTYPE_UINT8, &data, sizeof(uint8)); }
	virtual bool setPropUint16(const char* name, const uint16 data){ return setData(name, DTYPE_UINT16, &data, sizeof(uint16)); }
	virtual bool setPropUint32(const char* name, const uint32 data){ return setData(name, DTYPE_UINT32, &data, sizeof(uint32)); }
	virtual bool setPropUint64(const char* name, const uint64 data){ return setData(name, DTYPE_UINT64, &data, sizeof(uint64)); }
	
	virtual bool setPropFloat(const char* name, const float data){ return setData(name, DTYPE_FLOAT, &data, sizeof(float)); }
	virtual bool setPropDouble(const char* name, const double data){ return setData(name, DTYPE_DOUBLE, &data, sizeof(double)); }
	virtual bool setPropString(const char* name, const char* data){
		if (data == nullptr)
			return setData(name, DTYPE_STRING, "", (int32)strlen("") + 1);
		return setData(name, DTYPE_STRING, data, (int32)strlen(data) + 1); 
	}
	virtual bool setPropStruct(const char* name, const void* data, const int32 size) { return setData(name, DTYPE_STRUCT, data, size); }
	virtual bool setPropBlob(const char* name, const void* data, const int32 size) { return setData(name, DTYPE_BLOB, data, size); }

	virtual int8 getPropInt8(const char* name) const {int32 size = sizeof(int8); return *(int8*)getData(name, DTYPE_INT8, size);}
	virtual int16 getPropInt16(const char* name) const {int32 size = sizeof(int16); return *(int16*)getData(name, DTYPE_INT16, size);}
	virtual int32 getPropInt32(const char* name) const {int32 size = sizeof(int32); return *(int32*)getData(name, DTYPE_INT32, size);}
	virtual int64 getPropInt64(const char* name) const {int32 size = sizeof(int64); return *(int64*)getData(name, DTYPE_INT64, size);}

	virtual uint8 getPropUint8(const char* name) const {int32 size = sizeof(uint8); return *(uint8*)getData(name, DTYPE_UINT8, size);}
	virtual uint16 getPropUint16(const char* name) const {int32 size = sizeof(uint16); return *(uint16*)getData(name, DTYPE_UINT16, size);}
	virtual uint32 getPropUint32(const char* name) const {int32 size = sizeof(uint32); return *(uint32*)getData(name, DTYPE_UINT32, size);}
	virtual uint64 getPropUint64(const char* name) const {int32 size = sizeof(uint64); return *(uint64*)getData(name, DTYPE_UINT64, size); }

	virtual float getPropFloat(const char* name) const {int32 size = sizeof(float); return *(float*)getData(name, DTYPE_FLOAT, size);}
	virtual double getPropDouble(const char* name) const {int32 size = sizeof(double); return *(float*)getData(name, DTYPE_DOUBLE, size);}
	virtual const char* getPropString(const char* name) const {int32 size = 0; return (const char*)getData(name, DTYPE_STRING, size);}
	virtual const void* getPropStruct(const char* name, int32& size) const {size = 0; return (const void*)getData(name, DTYPE_STRUCT, size);}
	virtual const void* getPropBlob(const char* name, int32& size) const {size = 0; return (const void*)getData(name, DTYPE_BLOB, size);}
	virtual const void* getPropData(const char* name, int32& size) const {size = 0; return (const void*)getData(name, -1, size);}

	virtual IDict* getPropDict(const char* name) const {int32 size = 0; return (IDict*)getData(name, DTYPE_DICT, size);}
	virtual IArray* getPropArray(const char* name) const {int32 size = 0; return (IArray*)getData(name, DTYPE_ARRAY, size);}

protected:
	bool setData(const char* name, const int8 type, const void* data, const int32 size);
	const void* getData(const char* name, const int8 type, int32& size) const;

private:
	const MMObject*	_object;
	const std::unordered_map<std::string, PropLayout*>& _dictLayouts;
	OMemory*	_memory;
};
#endif
