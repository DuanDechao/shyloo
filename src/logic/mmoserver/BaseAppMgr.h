#ifndef __SL_FRAMEWORK_BASEAPPMGR_H__
#define __SL_FRAMEWORK_BASEAPPMGR_H__
#include "slsingleton.h"
#include "IMmoServer.h"
#include "slbinary_stream.h"
#include "slstring.h"
#include "GameDefine.h"

class BaseAppMgr : public IBaseAppMgr, public sl::SLHolder<BaseAppMgr>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);
	
	void onBaseMsgReqCreateBaseAnywhere(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const sl::OBStream& args);

private:
	sl::api::IKernel*		_kernel;
	BaseAppMgr*		        _self;
};
#endif
