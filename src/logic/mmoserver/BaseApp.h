#ifndef __SL_FRAMEWORK_BASEAPP_H__
#define __SL_FRAMEWORK_BASEAPP_H__
#include "slsingleton.h"
#include "IMmoServer.h"
#include "slbinary_stream.h"
#include "slstring.h"
#include "GameDefine.h"
#include "IGate.h"

class IObject;
class BaseApp : public IBaseApp, public IGateListener, public sl::SLHolder<BaseApp>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

    virtual bool remoteCreateCellEntity(IObject* object, const int32 cellIdx);
    virtual bool createEntityFromDB(const char* entityName, const uint64 dbid, const uint64 callbackId, const uint64 entityId);
	
    virtual void onGateLogined(sl::api::IKernel* pKernel, const int64 id);
	virtual void onGateLogOnAttempt(sl::api::IKernel* pKernel, const int64 id);
	virtual int32 onGateMsgRecv(sl::api::IKernel* pKernel, const int64 id, const void* context, const int32 size);
    virtual void onGateUnbind(sl::api::IKernel* pKernel, const int64 id);
    
    void onCellEntityCreatedFromCell(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);
    void onDBMsgQueryEntityCallback(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);

private:
	sl::api::IKernel*   _kernel;
	BaseApp*		    _self;
};
#endif
