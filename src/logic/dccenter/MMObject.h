#ifndef SL_MMOBJECT_H
#define SL_MMOBJECT_H
#include "IDCCenter.h"
#include "ObjectStruct.h"

class MMObject : public IObject{
public:
	MMObject(const char* name, ObjectPropInfo* pPropInfo) : m_poPropInfo(pPropInfo){
		SLASSERT(m_poPropInfo && name, "wtf");
		SafeSprintf(m_name, sizeof(m_name), name);
		m_poProp = NEW ObjectProp(pPropInfo);
	}

	~MMObject(){
		DEL m_poProp;
		m_poProp = nullptr;
		sl::SafeMemset(m_name, sizeof(m_name), 0, sizeof(int32));
	}

	virtual bool setPropInt8(const int32 prop, const int8 value);
	virtual bool setPropInt16(const int32 prop, const int16 value);
	virtual bool setPropInt32(const int32 prop, const int32 value);
	virtual bool setPropInt64(const int32 prop, const int64 value);
	virtual bool setPropFloat(const int32 prop, const float value);
	virtual bool setPropString(const int32 prop, const char* value);

	virtual int8 getPropInt8(const int32 prop) const;
	virtual int16 getPropInt16(const int32 prop) const;
	virtual int32 getPropInt32(const int32 prop) const;
	virtual int64 getPropInt64(const int32 prop) const;
	virtual float getPropFloat(const int32 prop) const;
	virtual const char* getPropString(const int32 prop) const;

private:
	char				m_name[256];
	ObjectPropInfo*		m_poPropInfo;
	ObjectProp*			m_poProp;

};
#endif