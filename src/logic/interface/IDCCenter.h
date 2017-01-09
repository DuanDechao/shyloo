#ifndef SL_IDCCENTER_H
#define SL_IDCCENTER_H
#include "slmulti_sys.h"
#include "slimodule.h"
#include "slstring.h"
#include "GameDefine.h"
#include "Attr.h"
#include <vector>

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

struct PropInfo{
	PropInfo():_name(0), _realname(""), _index(0), _offset(0), _size(0), _mask(0), _setting(0){}
	PropInfo(const int32 name, const char* realname, const int32 index, const int32 offset, const int32 size, const int8 mask, const int32 setting)
	:_name(name), _realname(realname), _index(index), _offset(offset), _size(size), _mask(mask), _setting(setting){}

	PropInfo& operator = (const PropInfo& info){
		_name = info._name;
		_realname = info._realname;
		_index = info._index;
		_offset = info._offset;
		_size = info._size;
		_mask = info._mask;
		_setting = info._setting;
		return *this;
	}

	int32 _name;
	sl::SLString<game::MAX_PROP_NAME_LEN> _realname;
	int32 _index;
	int32 _offset;
	int32 _size;
	int8 _mask;
	int32 _setting;
};

class IRow{
public:
	virtual int32 getRowIndex() const = 0;

	virtual int8 getDataInt8(const int32 col) const = 0;
	virtual int16 getDataInt16(const int32 col) const = 0;
	virtual int32 getDataInt32(const int32 col) const = 0;
	virtual int64 getDataInt64(const int32 col) const = 0;
	virtual float getDataFloat(const int32 col) const = 0;
	virtual const char * getDataString(const int32 col) const = 0;

	virtual void setDataInt8(const int32 col, const int8 value) = 0;
	virtual void setDataInt16(const int32 col, const int16 value) = 0;
	virtual void setDataInt32(const int32 col, const int32 value) = 0;
	virtual void setDataInt64(const int32 col, const int64 value) = 0;
	virtual void setDataFloat(const int32 col, const float value) = 0;
	virtual void setDataString(const int32 col, const char * value) = 0;
};

class IObject{
public:
	virtual bool setPropInt8(const int32 prop, const int8 value) = 0;
	virtual bool setPropInt16(const int32 prop, const int16 value) = 0;
	virtual bool setPropInt32(const int32 prop, const int32 value) = 0;
	virtual bool setPropInt64(const int32 prop, const int64 value) = 0;
	virtual bool setPropFloat(const int32 prop, const float value) = 0;
	virtual bool setPropString(const int32 prop, const char* value) = 0;

	virtual int8 getPropInt8(const int32 prop) const = 0;
	virtual int16 getPropInt16(const int32 prop) const = 0;
	virtual int32 getPropInt32(const int32 prop) const = 0;
	virtual int64 getPropInt64(const int32 prop) const = 0;
	virtual float getPropFloat(const int32 prop) const = 0;
	virtual const char* getPropString(const int32 prop) const = 0;

};
class IObjectMgr : public sl::api::IModule
{
public:
	virtual ~IObjectMgr() {}
};
#endif