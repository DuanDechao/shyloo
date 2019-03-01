#ifndef SL_CORE_DATA_ARRAY_H
#define SL_CORE_DATA_ARRAY_H
#include "MMObject.h"
class DataArray: public IArray{
public:
	DataArray(const MMObject* object, const PropLayout* layout, OMemory* memory)
		:_layout(layout),
		 _object(object),
		 _memory(memory)
	{}

	virtual ~DataArray(){}

	virtual int32 size() { return *(int32*)(_memory->buf());}
	virtual void remove(const int32 index) {}
	virtual bool setPropInt8(const int32 index, const int8 data){ return setData(index, DTYPE_INT8, &data, sizeof(int8)); }
	virtual bool setPropInt16(const int32 index, const int16 data){ return setData(index, DTYPE_INT16, &data, sizeof(int16)); }
	virtual bool setPropInt32(const int32 index, const int32 data){ return setData(index, DTYPE_INT32, &data, sizeof(int32)); }
	virtual bool setPropInt64(const int32 index, const int64 data){ return setData(index, DTYPE_INT64, &data, sizeof(int64)); }

	virtual bool setPropUint8(const int32 index, const uint8 data){ return setData(index, DTYPE_UINT8, &data, sizeof(uint8)); }
	virtual bool setPropUint16(const int32 index, const uint16 data){ return setData(index, DTYPE_UINT16, &data, sizeof(uint16)); }
	virtual bool setPropUint32(const int32 index, const uint32 data){ return setData(index, DTYPE_UINT32, &data, sizeof(uint32)); }
	virtual bool setPropUint64(const int32 index, const uint64 data){ return setData(index, DTYPE_UINT64, &data, sizeof(uint64)); }
	
	virtual bool setPropFloat(const int32 index, const float data){ return setData(index, DTYPE_FLOAT, &data, sizeof(float)); }
	virtual bool setPropDouble(const int32 index, const double data){ return setData(index, DTYPE_DOUBLE, &data, sizeof(double)); }
	virtual bool setPropString(const int32 index, const char* data){
		if (data == nullptr)
			return setData(index, DTYPE_STRING, "", (int32)strlen("") + 1);
		return setData(index, DTYPE_STRING, data, (int32)strlen(data) + 1); 
	}
	virtual bool setPropStruct(const int32 index, const void* data, const int32 size) { return setData(index, DTYPE_STRUCT, data, size); }
	virtual bool setPropBlob(const int32 index, const void* data, const int32 size) { return setData(index, DTYPE_BLOB, data, size); }

	virtual int8 getPropInt8(const int32 index) const {int32 size = sizeof(int8); return *(int8*)getData(index, DTYPE_INT8, size);}
	virtual int16 getPropInt16(const int32 index) const {int32 size = sizeof(int16); return *(int16*)getData(index, DTYPE_INT16, size);}
	virtual int32 getPropInt32(const int32 index) const {int32 size = sizeof(int32); return *(int32*)getData(index, DTYPE_INT32, size);}
	virtual int64 getPropInt64(const int32 index) const {int32 size = sizeof(int64); return *(int64*)getData(index, DTYPE_INT64, size);}

	virtual uint8 getPropUint8(const int32 index) const {int32 size = sizeof(uint8); return *(uint8*)getData(index, DTYPE_UINT8, size);}
	virtual uint16 getPropUint16(const int32 index) const {int32 size = sizeof(uint16); return *(uint16*)getData(index, DTYPE_UINT16, size);}
	virtual uint32 getPropUint32(const int32 index) const {int32 size = sizeof(uint32); return *(uint32*)getData(index, DTYPE_UINT32, size);}
	virtual uint64 getPropUint64(const int32 index) const {int32 size = sizeof(uint64); return *(uint64*)getData(index, DTYPE_UINT64, size); }

	virtual float getPropFloat(const int32 index) const {int32 size = sizeof(float); return *(float*)getData(index, DTYPE_FLOAT, size);}
	virtual double getPropDouble(const int32 index) const {int32 size = sizeof(double); return *(float*)getData(index, DTYPE_DOUBLE, size);}
	virtual const char* getPropString(const int32 index) const {int32 size = 0; return (const char*)getData(index, DTYPE_STRING, size);}
	virtual const void* getPropStruct(const int32 index, int32& size) const {size = 0; return (const void*)getData(index, DTYPE_STRUCT, size);}
	virtual const void* getPropBlob(const int32 index, int32& size) const {size = 0; return (const void*)getData(index, DTYPE_BLOB, size);}
	virtual const void* getPropData(const int32 index, int32& size) const {size = 0; return (const void*)getData(index, -1, size);}

	virtual IDict* getPropDict(const int32 index) const {int32 size = 0; return (IDict*)getData(index, DTYPE_DICT, size);}
	virtual IArray* getPropArray(const int32 index) const {int32 size = 0; return (IArray*)getData(index, DTYPE_ARRAY, size);}

protected:
	bool setData(const int32 index, const int8 type, const void* data, const int32 size);
	const void* getData(const int32 index, const int8 type, int32& size) const;

private:
	const MMObject*	 _object;
	const PropLayout*	 _layout;
	OMemory*	 _memory;
};
#endif
