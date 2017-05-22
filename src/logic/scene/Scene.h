#ifndef __SL_FRAMEWORK_SCENE_H__
#define __SL_FRAMEWORK_SCENE_H__
#include "slsingleton.h"
#include "IScene.h"
class IHarbor;
class IObjectMgr;
class ICapacitySubscriber;
class ICapacityPublisher;
class ITableControl;
class OArgs;
class IObject;
class IEventEngine;
class Scene : public IScene, public sl::SLHolder<Scene>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	void onSceneMgrCreateScene(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);
	void onSceneMgrEnterScene(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);
	void onSceneMgrAppearScene(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);
	void onSceneMgrLeaveScene(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);
	void onSceneMgrSyncScene(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);

	void onObjectAppearOnScene(sl::api::IKernel* pKernel, const void* context, const int32 size);

private:
	void confirmScene(sl::api::IKernel* pKernel, const char* scene);
	IObject* findScene(const char* scene);

private:
	sl::api::IKernel* _kernel;
	Scene*		_self;
	IHarbor*		_harbor;
	IObjectMgr*		_objectMgr;
	ICapacitySubscriber*  _capacitySubscriber;
	ICapacityPublisher* _capacityPublisher;
	IEventEngine*		_eventEngine;

	ITableControl*	_scenes;
};
#endif