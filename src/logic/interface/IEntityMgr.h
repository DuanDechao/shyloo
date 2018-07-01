#ifndef _SL_INTERFACE_ENTITYMGR_H__
#define _SL_INTERFACE_ENTITYMGR_H__
#include "slimodule.h"
#include "slbinary_stream.h"
class IObject;
class IEntityMgr : public sl::api::IModule{
public:
	virtual ~IEntityMgr() {}
	virtual void onRemoteMethodCall(const uint64 objectId, const sl::OBStream& stream)= 0;
};

#endif
