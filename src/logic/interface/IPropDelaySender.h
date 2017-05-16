#ifndef _SL_INTERFACE_PROPDELAYSENDER_H__
#define _SL_INTERFACE_PROPDELAYSENDER_H__
#include "slimodule.h"
class IObject;
class IProp;
class IPropDelaySender : public sl::api::IModule{
public:
	virtual ~IPropDelaySender() {}

	virtual void syncChangedProps(IObject* object) = 0;
	virtual void removeChangedProp(IObject* object, const IProp* prop) = 0;
};

#endif