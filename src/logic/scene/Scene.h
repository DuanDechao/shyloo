#ifndef __SL_FRAMEWORK_SCENE_H__
#define __SL_FRAMEWORK_SCENE_H__
#include "slsingleton.h"
#include "IScene.h"
#include <unordered_map>

class IHarbor;
class IObjectMgr;
class ICapacitySubscriber;
class ICapacityPublisher;
class ITableControl;
class OArgs;
class IObject;
class IEventEngine;
class Scene : public IScene, public sl::SLHolder<Scene>{
	struct SceneObjectNode{
		SceneObjectNode() :xPrev(nullptr), xNext(nullptr), yPrev(nullptr), yNext(nullptr), object(nullptr){}
		~SceneObjectNode(){}

		SceneObjectNode* xPrev;
		SceneObjectNode* xNext;
		SceneObjectNode* yPrev;
		SceneObjectNode* yNext;
		IObject* object;
	};

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

	void addObjectToScene(const char* sceneId, IObject* object);

private:
	typedef std::unordered_map<sl::SLString<MAX_SCENE_LEN>, SceneObjectNode*, sl::HashFunc<MAX_SCENE_LEN>, sl::EqualFunc<MAX_SCENE_LEN>> SCENE_OBJECTNODE_MAP;
	sl::api::IKernel* _kernel;
	Scene*		_self;
	IHarbor*		_harbor;
	IObjectMgr*		_objectMgr;
	ICapacitySubscriber*  _capacitySubscriber;
	ICapacityPublisher* _capacityPublisher;
	IEventEngine*		_eventEngine;

	ITableControl*	_scenes;
	SCENE_OBJECTNODE_MAP _sceneObjectNodes;
};
#endif