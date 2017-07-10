#ifndef __SL_FRAMEWORK_SCENEMGR_H__
#define __SL_FRAMEWORK_SCENEMGR_H__
#include "slsingleton.h"
#include "ISceneMgr.h"
class IHarbor;
class IObjectMgr;
class ICapacitySubscriber;
class ITableControl;
class SceneMgr: public ISceneMgr, public sl::SLHolder<SceneMgr>{
	enum SceneState{
		SS_NONE = 0,
		SS_DISTRIBUTE = 1,
		SS_CONFIRMED = 2,
	};
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	void onLogicEnterScene(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onLogicAppearScene(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onLogicLeaveScene(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onLogicUpdatePosition(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onSceneConfirmScene(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);

private:
	void distributeScene(sl::api::IKernel* pKernel, const char* scene);
	void addObjectToScene(sl::api::IKernel* pKernel, const char* scene, int64 id, float x, float y, float z, int32 gate, int32 logic, float vision);
	void appearObjectToScene(sl::api::IKernel* pKernel, const char* scene, int64 id, int32 gate);
	void removeObjectFromScene(sl::api::IKernel* pKernel, const char* scene, int64 id);

private:
	sl::api::IKernel* _kernel;
	SceneMgr*		_self;
	IHarbor*		_harbor;
	IObjectMgr*		_objectMgr;
	ICapacitySubscriber*  _capacity;

	ITableControl*  _objects;
	ITableControl*	_scenes;
};
#endif