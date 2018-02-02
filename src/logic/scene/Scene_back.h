#ifndef __SL_FRAMEWORK_SCENE_H__
#define __SL_FRAMEWORK_SCENE_H__
#include "slsingleton.h"
#include "IScene.h"
#include <unordered_map>
#include "slstring.h"
#include "GameDefine.h"

class IHarbor;
class IObjectMgr;
class ICapacitySubscriber;
class ICapacityPublisher;
class ITableControl;
class OArgs;
class IObject;
class IEventEngine;
class Scene : public IScene, public sl::SLHolder<Scene>{
	enum Quadrant{
		X = 1,
		Y,
		Z,
	};

	struct SceneEntity{
		SceneEntity() :prev(nullptr), next(nullptr), object(nullptr), isHead(false){}
		SceneEntity* prev;
		SceneEntity* next;
		IObject* object;
		bool isHead;
	};
	
	struct SceneNodesList{
		SceneNodesList(){
			xListHead.isHead = true;
			yListHead.isHead = true;
		}
		SceneEntity xListHead;
		SceneEntity yListHead;
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

	void onPlayerEnterScene(sl::api::IKernel* pKernel, const void* context, const int32 size);

private:
	typedef std::function<void(sl::api::IKernel* pKernel, IObject* object)> VisionEvent;
	void confirmScene(sl::api::IKernel* pKernel, const char* scene);
	IObject* findScene(const char* scene);

	void objectEnterVision(IObject* object, IObject* other);
	void objectLeaveVision(IObject* object, IObject* other);
	void objectMoveInVision(IObject* object, IObject* other);

	void notifyLogicAddWatcher(IObject* object, IObject* watcher);
	void notifyLogicRemoveWatcher(IObject* object, IObject* watcher);
	void notifyLogicAddInterester(IObject* object, IObject* interester);
	void notifyLogicRemoveInterester(IObject* object, IObject* interester);

	void addObjectToScene(IObject* object, const VisionEvent& add);
	void removeObjectOnScene(IObject* object, const VisionEvent& remove);
	void moveObjectOnScene(IObject* object, const VisionEvent& add, const VisionEvent& remove, const VisionEvent& move);
	void foreachVisionObject(IObject* object, int8 quadrant, const VisionEvent& func);
	void insertSceneNode(SceneEntity* head, SceneEntity* node, const std::function<bool(const SceneEntity* innerNode)>& conditionFunc, bool forward = false);
	void moveSceneNode(SceneEntity* head, SceneEntity* node, int8 quadrant, bool forward = false);
	void removeSceneNode(SceneEntity* node);
	bool isInVision(IObject* object, IObject* other);

	void printSceneNodePos();

private:
	typedef std::unordered_map<sl::SLString<game::MAX_SCENE_LEN>, SceneNodesList> SCENE_OBJECTNODE_MAP;
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
