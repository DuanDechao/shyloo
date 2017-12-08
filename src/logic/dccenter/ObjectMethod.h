#ifndef __SL_OBJECT_METHOD_H__
#define __SL_OBJECT_METHOD_H__
#include "sltype.h"
#include "Memory.h"
#include "slstring.h"
#include "IDCCenter.h"
#include "sltools.h"
#include "ObjectStruct.h"
class IObject;
class ObjectMethod : public IMethod{
public:
	ObjectMethod(int32 name, int32 size) :_name(name), _size(size){
		int32 mallocSize = _size * sizeof(MethodLayout*);
		_layouts = (MethodLayout**)SLMALLOC(mallocSize);
		sl::SafeMemset(_layouts, mallocSize, 0, mallocSize);
	}

	virtual ~ObjectMethod(){
		SLFREE(_layouts);
	}

	virtual const int32 getName() const { return _name; }
	virtual const int8 getType(IObject* object) const;
	virtual const int32 getSetting(IObject* object) const;
	virtual const int32 getIndex(IObject* object) const;
	virtual std::vector<uint8> getArgs(IObject* object) const;

	inline void setLayout(const int32 objTypeId, MethodLayout* layout){
		//SLASSERT(objTypeId > 0 && objTypeId <= _size, "out of range");
		if (objTypeId > 0 && objTypeId <= _size)
			_layouts[objTypeId - 1] = layout;
	}

	inline const MethodLayout* getLayout(const int32 objTypeId){
		//SLASSERT(objTypeId > 0 && objTypeId <= _size, "out of range");
		if (objTypeId > 0 && objTypeId <= _size)
			return _layouts[objTypeId - 1];
		return nullptr;
	}
private:
	MethodLayout**	_layouts;
	int32			_name;
	int32			_size;
};
#endif
