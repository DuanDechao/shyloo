#ifndef _SL_INTERFACE_MMOSERVER_H__
#define _SL_INTERFACE_MMOSERVER_H__
#include "slimodule.h"
#include <functional>
class IObject;
class IBaseApp : public sl::api::IModule{
public:
	virtual ~IBaseApp() {}
    virtual bool remoteCreateCellEntity(IObject* object, const int32 cellIdx, const void* cellData, const int32 cellDataSize) = 0;
    virtual bool createEntityFromDB(const char* entityName, const uint64 dbid, const uint64 callbackId, const uint64 entityId = 0) = 0;
};

class ICellApp : public sl::api::IModule{
public:
	virtual ~ICellApp() {}
    virtual bool createEntity(const char* entityName, const uint64 entityId = 0) = 0;
};

class IDBMgr : public sl::api::IModule{
public:
	virtual ~IDBMgr() {}
};
#endif
