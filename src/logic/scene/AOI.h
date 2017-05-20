#ifndef __SL_FRAMEWORK_AOI_H__
#define __SL_FRAMEWORK_AOI_H__
#include "slsingleton.h"
#include "IAOI.h"
class IHarbor;
class IObjectMgr;
class AOI : public IAOI, public sl::SLHolder<AOI>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	void onSceneAddInterester(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);
	void onSceneAddWatcher(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);
	void onSceneRemoveInterester(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);
	void onSceneRemoveWatcher(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);

private:
	sl::api::IKernel* _kernel;
	AOI*			  _self;
	IHarbor*		  _harbor;
	IObjectMgr*		  _objectMgr;
};
#endif