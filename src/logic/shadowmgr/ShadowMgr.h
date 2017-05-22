#ifndef __SL_FRAMEWORK_SHADOWMGR_H__
#define __SL_FRAMEWORK_SHADOWMGR_H__
#include "IShadowMgr.h"
#include "slsingleton.h"
#include "slbinary_stream.h"
class IHarbor;
class IObject;
class IObjectMgr;
class IEventEngine;
class IProp;
class ShadowMgr :public IShadowMgr, public sl::SLHolder<ShadowMgr>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void createShadow(IObject* object, const int32 logic);
	virtual void delShadow(IObject* object, const int32 logic);

	void onLogicCreateShadow(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args);
	void onLogicSyncShadow(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args);
	void onLogicDestroyShadow(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args);

	void syncShadow(sl::api::IKernel* pKernel, IObject* object, const char* name, const IProp* prop, const bool sync);

private:
	void sendCreateShadow(sl::api::IKernel* pKernel, IObject* object, const int32 logic);
	void sendDestroyShadow(sl::api::IKernel* pKernel, IObject* object, const int32 logic);
	void brocastShadowPropSync(sl::api::IKernel* pKernel, IObject* object, const std::function<void(sl::api::IKernel* pKernel, const int32 logic)>& f);

private:
	ShadowMgr* _self;
	sl::api::IKernel* _kernel;
	IHarbor* _harbor;
	IObjectMgr* _objectMgr;
	IEventEngine* _eventEngine;
};
#endif