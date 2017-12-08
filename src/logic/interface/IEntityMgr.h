#ifndef _SL_INTERFACE_ENTITYMGR_H__
#define _SL_INTERFACE_ENTITYMGR_H__
#include "slimodule.h"
#include "slpyscript.h"

class IObject;
class IBase{
public:
	virtual ~IBase() {}
};

class IEntityMgr : public sl::api::IModule{
public:
	virtual ~IEntityMgr() {}
	virtual IObject* createBase(const char* entityType, PyObject* params) = 0;
};

#endif