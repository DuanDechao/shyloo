#ifndef __SL_FRAMEWORK_CELLAPP_H__
#define __SL_FRAMEWORK_CELLAPP_H__
#include "slsingleton.h"
#include "IMmoServer.h"
#include "slbinary_stream.h"
#include "slstring.h"
#include "GameDefine.h"
#include "IGlobalData.h"
class ISpace;
class IProp;
class CellApp : public ICellApp, public IGlobalDataListener{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);
    
    void onCreateCellEntityFromBase(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);
    virtual bool createEntity(const char* entityName, const uint64 entityId = 0);
    virtual bool addSpaceGeometryMapping(const int32 spaceId, const char* path, bool shouldLoadOnServer, std::map<int32, std::string>& params);

    void test();
    void timerStart(sl::api::IKernel* pKernel, IObject* object, int64 tick);
    void onTimer(sl::api::IKernel* pKernel, IObject* object, int64 tick);
    void timerEnd(sl::api::IKernel* pKernel, IObject* object, bool novolient, int64 tick);
	virtual void onGlobalDataChanged(const char* key, const int16 dataType, const void* data, const int32 dataSize, bool isDelete);

private:
    void onGetWitness(IObject* object, ISpace* space);
    void addSpaceDataToClient(IObject* object, ISpace* space);
    void onEnterSpace(IObject* object);
    bool createCellEntityFromBase(ISpace* space, const char* entityType, const int64 entityId, bool hasClient, int32 baseNodeId, const void* cellData, const int32 cellDataSize);

private:
	sl::api::IKernel*   _kernel;
	CellApp*		    _self;
    const IProp* _propPosChangeTimer;
};
#endif
