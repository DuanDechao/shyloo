#ifndef _SL_INTERFACE_ENTITYMGR_H__
#define _SL_INTERFACE_ENTITYMGR_H__
#include "slimodule.h"
class IObject;
class IEntityMgr : public sl::api::IModule{
public:
	virtual ~IEntityMgr() {}
    virtual IObject* createCellEntity(const char* entityType, const void* cellData, const int32 cellDataSize, const int32 baseNodeId, const uint64 entityId = 0) = 0;
};

#endif
