#ifndef _SL_INTERFACE_MMOSERVER_H__
#define _SL_INTERFACE_MMOSERVER_H__
#include "slimodule.h"
#include <functional>
#include "slbinary_stream.h"
#include <map>
#include "slmath.h"
class IObject;
class IBaseApp : public sl::api::IModule{
public:
	virtual ~IBaseApp() {}
	virtual bool remoteCreateCellEntity(IObject* object, const uint64 createToObjectId, const int32 cellIdx, bool hasClient, const void* cellData, const int32 cellDataSize) = 0;
	virtual bool createBaseAnywhere(const char* entityType, const void* initData, const int32 initDataSize, const uint64 callbackId) = 0;
    virtual bool createEntityFromDB(const char* entityName, const uint64 dbid, const uint64 callbackId, const uint64 entityId = 0) = 0;
    virtual void proxyCreated(IObject* object, const int64 id) = 0;
};

class ICellApp : public sl::api::IModule{
public:
	virtual ~ICellApp() {}
    virtual IObject* createEntity(const char* entityName, const int32 spaceId, Position3D& pos, Position3D& dir, const void* initData, const int32 initDataSize) = 0;
    virtual bool addSpaceGeometryMapping(const int32 spaceId, const char* path, bool shouldLoadOnServer, std::map<int32, std::string>& params) = 0;
};

class IDBMgr : public sl::api::IModule{
public:
	virtual ~IDBMgr() {}
};

class IBaseAppMgr: public sl::api::IModule{
public:
	virtual ~IBaseAppMgr() {}
};

class ICellAppMgr: public sl::api::IModule{
public:
	virtual ~ICellAppMgr() {}
};
#endif
