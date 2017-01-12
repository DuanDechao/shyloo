#ifndef __SL_OBJECT_PROP_H__
#define __SL_OBJECT_PROP_H__
#include "sltype.h"
#include "Memory.h"
#include "slstring.h"
#include "IDCCenter.h"
#include "sltools.h"
#include "ObjectStruct.h"
class IObject;
class ObjectProp{
public:
	ObjectProp(int32 name, int32 size) :m_name(name), m_size(size){
		int32 mallocSize = m_size * sizeof(PropLayout*);
		m_layouts = (PropLayout**)MALLOC(mallocSize);
		sl::SafeMemset(m_layouts, mallocSize, 0, mallocSize);
	}

	~ObjectProp(){
		FREE(m_layouts);
	}

	virtual const int32 getName() const { return m_name; }
	virtual const int8 getType(IObject* object) const;
	virtual const int32 getSetting(IObject* object) const;

	inline void setLayout(const int32 objTypeId, PropLayout* layout){
		SLASSERT(objTypeId > 0 && objTypeId <= m_size, "out of range");
		if (objTypeId > 0 && objTypeId <= m_size)
			m_layouts[objTypeId - 1] = layout;
	}

	inline const PropLayout* getLayout(const int32 objTypeId){
		SLASSERT(objTypeId > 0 && objTypeId <= m_size, "out of range");
		if (objTypeId > 0 && objTypeId <= m_size)
			return m_layouts[objTypeId - 1];
		return nullptr;
	}
private:
	PropLayout**	m_layouts;
	int32			m_name;
	int32			m_size;
};
#endif