#ifndef _SL_INTERFACE_SHADOWMGR_H__
#define _SL_INTERFACE_SHADOWMGR_H__
#include "slimodule.h"
class IObject;
class IShadowMgr : public sl::api::IModule{
public:
	virtual ~IShadowMgr() {}

	virtual void createShadow(IObject* object, const int32 logic) = 0;
	virtual void removeShadow(IObject* object, const int32 logic) = 0; 
};

#endif