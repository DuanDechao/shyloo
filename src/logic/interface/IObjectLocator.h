#ifndef __SL_INTERFACE_OBJECTLOCATOR_H__
#define __SL_INTERFACE_OBJECTLOCATOR_H__
#include "slimodule.h"
class IObjectLocator : public sl::api::IModule{
public:
	virtual ~IObjectLocator() {}

	virtual int32 findObjectGate(int64 id) = 0;
	virtual int32 findObjectLogic(int64 id) = 0;
};
#endif