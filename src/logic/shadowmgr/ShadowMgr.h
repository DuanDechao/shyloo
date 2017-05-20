#ifndef __SL_FRAMEWORK_SHADOWMGR_H__
#define __SL_FRAMEWORK_SHADOWMGR_H__
#include "IShadowMgr.h"
#include "slsingleton.h"
class IHarbor;
class ShadowMgr :public IShadowMgr, public sl::SLHolder<ShadowMgr>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	void onSceneCreateShadow(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);
	void onLogicSyncShadow(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);

private:
	ShadowMgr* _self;
	sl::api::IKernel* _kernel;
	IHarbor* _harbor;
};
#endif