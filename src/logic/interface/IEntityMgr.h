#ifndef _SL_INTERFACE_ENTITYMGR_H__
#define _SL_INTERFACE_ENTITYMGR_H__
#include "slimodule.h"
#include "python3.4m/Python.h"

class IObject;
class IBase{
public:
	virtual ~IBase() {}
};

class IEntityMgr : public sl::api::IModule{
public:
	virtual ~IEntityMgr() {}
};

#endif
