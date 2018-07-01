#ifndef __SL_FRAMEWORK_DBMGR_H__
#define __SL_FRAMEWORK_DBMGR_H__
#include "slsingleton.h"
#include "IMmoServer.h"
#include "slbinary_stream.h"
#include "slstring.h"
#include "GameDefine.h"

class DBMgr : public IDBMgr, public sl::SLHolder<DBMgr>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

    void onBaseMsgQueryEntity(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);

private:
	sl::api::IKernel*   _kernel;
	DBMgr*		        _self;
};
#endif
