#include "Scene.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "IDCCenter.h"
#include "ICapacity.h"
#include "Attr.h"
#include "sltime.h"
#include "EventID.h"
#include "IEventEngine.h"
#include <set>

bool Scene::initialize(sl::api::IKernel * pKernel){
	_self = this;
	return true;
}

bool Scene::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);

	if (_harbor->getNodeType() != NodeType::SCENE)
		return true;

	FIND_MODULE(_objectMgr, ObjectMgr);
	FIND_MODULE(_capacitySubscriber, CapacitySubscriber);
	FIND_MODULE(_capacityPublisher, CapacityPublisher);
	FIND_MODULE(_eventEngine, EventEngine);

	_scenes = CREATE_STATIC_TABLE(_objectMgr, OCStaticTableMacro::SCENECOPY::TABLE_NAME, OCStaticTableMacro::SCENECOPY::TABLE_NAME);

	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::SCENEMGR_MSG_CREATE_SCENE, Scene::onSceneMgrCreateScene);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::SCENEMGR_MSG_ENTER_SCENE, Scene::onSceneMgrEnterScene);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::SCENEMGR_MSG_APPEAR_SCENE, Scene::onSceneMgrAppearScene);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::SCENEMGR_MSG_LEAVE_SCENE, Scene::onSceneMgrLeaveScene);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::SCENEMGR_MSG_SYNC_SCENE, Scene::onSceneMgrSyncScene);

	RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_SCENE_ENTER_SCENE, Scene::onPlayerEnterScene);

	return true;
}

bool Scene::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Scene::onSceneMgrCreateScene(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
	const char* scene = args.getString(0);
	if (_scenes->findRow(scene)){
		confirmScene(pKernel, scene);
		return;
	}

	IObject* sceneObj = CREATE_OBJECT(_objectMgr, "Scene");
	sceneObj->setPropString(attr_def::sceneId, scene);
	sceneObj->setPropInt64(attr_def::startTime, sl::getTimeMilliSecond());

	IRow* row = _scenes->addRowKeyString(scene);
	row->setDataInt64(OCStaticTableMacro::SCENECOPY::OBJECT, (int64)sceneObj);

	confirmScene(pKernel, scene);
	_capacityPublisher->increaseLoad(10);
}

void Scene::confirmScene(sl::api::IKernel* pKernel, const char* scene){
	IArgs<1, 128> args;
	args << scene;
	args.fix();

	_harbor->send(NodeType::SCENEMGR, 1, NodeProtocol::SCENE_MSG_SCENE_CONFIRMED, args.out());
}

void Scene::onSceneMgrEnterScene(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
	const char* scene = args.getString(0);
	int64 id = args.getInt64(1);
	float x = args.getFloat(2);
	float y = args.getFloat(3);
	float z = args.getFloat(4);
	int32 gate = args.getInt32(5);
	int32 logic = args.getInt32(6);
	float vision = args.getFloat(7);

	IObject* sceneObj = findScene(scene);
	SLASSERT(sceneObj, "%s is not exist", scene);
	if (!sceneObj)
		return;

	ITableControl* sceneObjects = sceneObj->findTable(OCTableMacro::SCENEOBJECTS::TABLE_NAME);
	SLASSERT(sceneObjects, "wtf");
	sceneObjects->addRowKeyInt64(id);

	IObject* sceneUnit = CREATE_OBJECT_BYID(_objectMgr, "SceneUnit", id);
	SLASSERT(sceneUnit, "wtf");
	sceneUnit->setPropString(attr_def::sceneId, scene);
	sceneUnit->setPropFloat(attr_def::x, x);
	sceneUnit->setPropFloat(attr_def::y, y);
	sceneUnit->setPropFloat(attr_def::z, z);
	sceneUnit->setPropFloat(attr_def::oldX, x);
	sceneUnit->setPropFloat(attr_def::oldY, y);
	sceneUnit->setPropFloat(attr_def::oldZ, z);
	sceneUnit->setPropInt32(attr_def::gate, gate);
	sceneUnit->setPropInt32(attr_def::logic, logic);
	sceneUnit->setPropFloat(attr_def::vision, vision);

	addObjectToScene(sceneUnit, [&](sl::api::IKernel* pKernel, IObject* other){
		objectEnterVision(sceneUnit, other);
	});

	logic_event::EnterVision evt;
	evt.object = sceneObj;
	evt.id = id;
	_eventEngine->execEvent(logic_event::EVENT_SCENE_ENTER_SCENE, &evt, sizeof(evt));

	_capacityPublisher->increaseLoad(1);
}

void Scene::onSceneMgrAppearScene(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
	const char* sceneId = args.getString(0);
	int64 id = args.getInt64(1);
	int32 gate = args.getInt32(2);

	IObject* sceneObj = findScene(sceneId);
	SLASSERT(sceneObj, "where is scene %s", sceneId);
	if (!sceneObj)
		return;

	IObject* object = _objectMgr->findObject(id);
	SLASSERT(object && strcmp(object->getPropString(attr_def::sceneId), sceneObj->getPropString(attr_def::id)) == 0, "wtf");
	if (!object)
		return;

#ifdef _DEBUG
	ITableControl* objects = sceneObj->findTable(OCTableMacro::SCENEOBJECTS::TABLE_NAME);
	SLASSERT(objects, "wtf");

	const IRow* objectRow = objects->findRow(id);
	if (!objectRow){
		SLASSERT(false, "can't find object[%lld] in scene[%s]", id, sceneId);
		return;
	}
#endif

	SLASSERT(object->getPropInt8(attr_def::appear) == 0, "wtf");
	if (object->getPropInt8(attr_def::appear) == 1)
		return;

	object->setPropInt8(attr_def::appear, 1);
	object->setPropInt32(attr_def::gate, gate);

	logic_event::AppearVision evt;
	evt.object = sceneObj;
	evt.id = id;
	_eventEngine->execEvent(logic_event::EVENT_SCENE_APPEAR_SCENE, &evt, sizeof(evt));
}

void Scene::onSceneMgrLeaveScene(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
	const char* sceneId = args.getString(0);
	int64 id = args.getInt64(1);

	IObject* sceneObj = findScene(sceneId);
	SLASSERT(sceneObj, "can't find scene %s", sceneId);
	if (!sceneObj)
		return;

	ITableControl* sceneObjectsTab = sceneObj->findTable(OCTableMacro::SCENEOBJECTS::TABLE_NAME);
	SLASSERT(sceneObjectsTab, "wtf");
	const IRow* findRow = sceneObjectsTab->findRow(id);
	if (!findRow){
		SLASSERT(false, "can't find object[%lld]", id);
		return;
	}

	IObject* sceneObject = _objectMgr->findObject(id);
	SLASSERT(sceneObject && strcmp(sceneObject->getPropString(attr_def::sceneId), sceneObj->getPropString(attr_def::id)) == 0, "wtf");
	if (!sceneObject)
		return;

	SLASSERT(sceneObject->getPropInt8(attr_def::appear) == 1, "wtf");
	if (sceneObject->getPropInt8(attr_def::appear) == 1){
	}

	DEL_TABLE_ROW(sceneObjectsTab, findRow);

	removeObjectOnScene(sceneObj, [&](sl::api::IKernel* pKernel, IObject* other){
		objectLeaveVision(sceneObj, other);
	});

	logic_event::LeaveVision evt;
	evt.object = sceneObj;
	evt.id = id;
	_eventEngine->execEvent(logic_event::EVENT_SCENE_LEAVE_SCENE, &evt, sizeof(evt));

	_objectMgr->recover(sceneObject);

	_capacityPublisher->decreaseLoad(1);
}

void Scene::onSceneMgrSyncScene(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
	const char* sceneId = args.getString(0);
	int64 id = args.getInt64(1);
	float x = args.getFloat(2);
	float y = args.getFloat(3);
	float z = args.getFloat(4);

	IObject* sceneObj = findScene(sceneId);
	if (!sceneObj){
		SLASSERT(false, "wtf");
		return;
	}

	IObject* object = _objectMgr->findObject(id);
	if (!object || strcmp(object->getPropString(attr_def::sceneId), sceneObj->getPropString(attr_def::id)) != 0){
		SLASSERT(false, "wtf");
		return;
	}

#ifdef _DEBUG
	ITableControl* objects = sceneObj->findTable(OCTableMacro::SCENEOBJECTS::TABLE_NAME);
	SLASSERT(objects, "wtf");
	if (!objects->findRow(id)){
		SLASSERT(false, "wtf");
		return;
	}
#endif

	float oldX = object->getPropFloat(attr_def::x);
	float oldY = object->getPropFloat(attr_def::y);
	float oldZ = object->getPropFloat(attr_def::z);

	object->setPropFloat(attr_def::x, x);
	object->setPropFloat(attr_def::y, y);
	object->setPropFloat(attr_def::z, z);
	object->setPropFloat(attr_def::oldX, oldX);
	object->setPropFloat(attr_def::oldY, oldY);
	object->setPropFloat(attr_def::oldZ, oldZ);

	moveObjectOnScene(object, [&](sl::api::IKernel* pKernel, IObject* other){
		objectEnterVision(object, other);
	},[&](sl::api::IKernel* pKernel, IObject* other){
		objectLeaveVision(object, other);
	}, [&](sl::api::IKernel* pKernel, IObject* other){
		objectMoveInVision(object, other);
	});
}

void Scene::onPlayerEnterScene(sl::api::IKernel* pKernel, const void* context, const int32 size){
	//printSceneNodePos();
}

IObject* Scene::findScene(const char* scene){
	const IRow* row = _scenes->findRow(scene);
	if (!row)
		return nullptr;

	return (IObject*)row->getDataInt64(OCStaticTableMacro::SCENECOPY::OBJECT);
}

void Scene::objectEnterVision(IObject* object, IObject* other){
	notifyLogicAddInterester(object, other);
	notifyLogicAddWatcher(object, other);
	notifyLogicAddInterester(other, object);
	notifyLogicAddWatcher(other, object);
}

void Scene::objectLeaveVision(IObject* object, IObject* other){
	notifyLogicRemoveInterester(object, other);
	notifyLogicRemoveWatcher(object, other);
	notifyLogicRemoveInterester(other, object);
	notifyLogicRemoveWatcher(other, object);
}

void Scene::objectMoveInVision(IObject* object, IObject* other){

}

void Scene::notifyLogicAddWatcher(IObject* object, IObject* watcher){
	IArgs<6, 256> args;
	args << object->getID() << watcher->getID() << watcher->getPropInt32(attr_def::gate);
	args << watcher->getPropInt32(attr_def::logic);
	args.fix();

	_harbor->send(NodeType::LOGIC, object->getPropInt32(attr_def::logic), NodeProtocol::SCENE_MSG_ADD_WATCHER, args.out());
}

void Scene::notifyLogicRemoveWatcher(IObject* object, IObject* watcher){
	IArgs<6, 256> args;
	args << object->getID() << watcher->getID();
	args.fix();

	_harbor->send(NodeType::LOGIC, object->getPropInt32(attr_def::logic), NodeProtocol::SCENE_MSG_REMOVE_WATCHER, args.out());
}

void Scene::notifyLogicAddInterester(IObject* object, IObject* interester){
	IArgs<6, 256> args;
	args << object->getID() << interester->getID() << interester->getPropInt32(attr_def::type);
	args.fix();

	_harbor->send(NodeType::LOGIC, object->getPropInt32(attr_def::logic), NodeProtocol::SCENE_MSG_ADD_INTERESTER, args.out());
}

void Scene::notifyLogicRemoveInterester(IObject* object, IObject* interester){
	IArgs<6, 256> args;
	args << object->getID() << interester->getID();
	args.fix();

	_harbor->send(NodeType::LOGIC, object->getPropInt32(attr_def::logic), NodeProtocol::SCENE_MSG_REMOVE_INTERESTER, args.out());
}

void Scene::addObjectToScene(IObject* object, const VisionEvent& add){
	const char* sceneId = object->getPropString(attr_def::sceneId);

	SceneEntity* nodeX = NEW SceneEntity();
	nodeX->object = object;
	object->setTempInt64(OCTempProp::SCENE_X_NODE, (int64)nodeX);

	SceneEntity* nodeY = NEW SceneEntity();
	nodeY->object = object;
	object->setTempInt64(OCTempProp::SCENE_X_NODE, (int64)nodeY);

	if (_sceneObjectNodes[sceneId].xListHead.next == nullptr){
		_sceneObjectNodes[sceneId].xListHead.next = nodeX;
		nodeX->prev = &_sceneObjectNodes[sceneId].xListHead;
	}
	else{
		SceneEntity* head = _sceneObjectNodes[sceneId].xListHead.next;
		insertSceneNode(head, nodeX, [&](const SceneEntity* innerNode){
			if (innerNode->object->getPropFloat(attr_def::x) > nodeX->object->getPropFloat(attr_def::x))
				return true;
			return false;
		});
	}

	if (_sceneObjectNodes[sceneId].yListHead.next == nullptr){
		_sceneObjectNodes[sceneId].yListHead.next = nodeY;
		nodeY->prev = &_sceneObjectNodes[sceneId].yListHead;
	}
	else{
		SceneEntity* head = _sceneObjectNodes[sceneId].yListHead.next;
		insertSceneNode(head, nodeY, [&](const SceneEntity* innerNode){
			if (innerNode->object->getPropFloat(attr_def::y) > nodeY->object->getPropFloat(attr_def::y))
				return true;
			return false;
		});
	}

	foreachVisionObject(object, Quadrant::X, add);
}

void Scene::removeObjectOnScene(IObject* object, const VisionEvent& remove){
	SceneEntity* nodeX = (SceneEntity*)object->getTempInt64(OCTempProp::SCENE_X_NODE);
	SceneEntity* nodeY = (SceneEntity*)object->getTempInt64(OCTempProp::SCENE_Y_NODE);

	foreachVisionObject(object, Quadrant::X, remove);

	removeSceneNode(nodeX);
	removeSceneNode(nodeY);

	DEL nodeX;
	DEL nodeY;
	
	object->setTempInt64(OCTempProp::SCENE_X_NODE, 0);
	object->setTempInt64(OCTempProp::SCENE_Y_NODE, 0);
}

void Scene::moveObjectOnScene(IObject* object, const VisionEvent& add, const VisionEvent& remove, const VisionEvent& move){
	std::set<IObject*> beforeVisionObjects;
	foreachVisionObject(object, Quadrant::X, [&](sl::api::IKernel* pKernel, IObject* object){
		beforeVisionObjects.insert(object);
	});

	if (object->getPropFloat(attr_def::x) != object->getPropFloat(attr_def::oldX)){
		SceneEntity* nodeX = (SceneEntity*)object->getTempInt64(OCTempProp::SCENE_X_NODE);
		
		bool isForwardX = object->getPropFloat(attr_def::x) < object->getPropFloat(attr_def::oldX);
		SceneEntity* head = isForwardX ? nodeX->prev : nodeX->next;
		moveSceneNode(head, nodeX, Quadrant::X, isForwardX);
	}

	if (object->getPropFloat(attr_def::y) != object->getPropFloat(attr_def::oldY)){
		SceneEntity* nodeY = (SceneEntity*)object->getTempInt64(OCTempProp::SCENE_Y_NODE);

		bool isForwardY = object->getPropFloat(attr_def::y) < object->getPropFloat(attr_def::oldY);
		SceneEntity* head = isForwardY ? nodeY->prev : nodeY->next;
		moveSceneNode(head, nodeY, Quadrant::Y, isForwardY);
	}
	 
	float disX = abs(object->getPropFloat(attr_def::x) - object->getPropFloat(attr_def::oldX));
	float disY = abs(object->getPropFloat(attr_def::y) - object->getPropFloat(attr_def::oldY));
	foreachVisionObject(object, disX >= disY ? Quadrant::X : Quadrant::Y, [&](sl::api::IKernel* pKernel, IObject* other){
		auto itor = beforeVisionObjects.find(other);
		if (itor != beforeVisionObjects.end()){
			move(pKernel, other);
			beforeVisionObjects.erase(itor);
		}
		else{
			add(pKernel, other);
		}
	});

	if (!beforeVisionObjects.empty()){
		auto itor = beforeVisionObjects.begin();
		for (; itor != beforeVisionObjects.end(); ++itor){
			remove(_kernel, *itor);
		}
	}
}

void Scene::foreachVisionObject(IObject* object, int8 quadrant, const std::function<void(sl::api::IKernel* pKernel, IObject* object)>& func){
	SceneEntity* searchNode = nullptr;
	switch (quadrant){
	case Quadrant::X: searchNode = (SceneEntity*)object->getTempInt64(OCTempProp::SCENE_X_NODE); break;
	case Quadrant::Y: searchNode = (SceneEntity*)object->getTempInt64(OCTempProp::SCENE_Y_NODE); break;
	case Quadrant::Z: searchNode = (SceneEntity*)object->getTempInt64(OCTempProp::SCENE_Z_NODE); break;
	default: SLASSERT(false, "wtf"); break;
	}

	SceneEntity* currPrev = searchNode->prev;
	while (currPrev && !currPrev->isHead){
		if (!isInVision(object, currPrev->object))
			break;
		//ECHO_ERROR("prev foreach[%lld]-[%lld]", object->getID(), currPrev->object->getID());
		func(_kernel, currPrev->object);
		currPrev = currPrev->prev;
	}

	SceneEntity* currNext = searchNode->next;
	while (currNext){
		if (!isInVision(object, currNext->object))
			break;
		//ECHO_ERROR("next foreach[%lld]-[%lld]", object->getID(), currNext->object->getID());
		func(_kernel, currNext->object);
		currNext = currNext->next;
	}
}

void Scene::insertSceneNode(SceneEntity* head, SceneEntity* node, const std::function<bool(const SceneEntity* innerNode)>& conditionFunc, bool forward){
	SceneEntity * curr = head;
	bool isInsert = false;
	while (curr){
		if ((!forward && !curr->next) || (forward && curr->prev->isHead))
			break;

		SLASSERT(curr->object, "wtf");

		if ((!forward && conditionFunc(curr)) && (forward && conditionFunc(curr->prev))){
			node->next = curr;
			node->prev = curr->prev;
			curr->prev->next = node;
			curr->prev = node;
			isInsert = true;
			break;
		}

		curr = forward ? curr->prev : curr->next;
	}

	if (!isInsert && !forward && !curr->next){
		node->prev = curr;
		curr->next = node;
	}

	if (!isInsert && forward && curr->prev->isHead){
		node->next = curr;
		node->prev = curr->prev;
		curr->prev->next = node;
		curr->prev = node;
	}
}

void Scene::removeSceneNode(SceneEntity* node){
	node->prev->next = node->next;
	node->next->prev = node->prev;
	node->prev->next = node->next;
	node->next->prev = node->prev;

	node->prev = nullptr;
	node->next= nullptr;
}

void Scene::moveSceneNode(SceneEntity* head, SceneEntity* node, int8 quadrant, bool forward){
	removeSceneNode(node);
	insertSceneNode(head, node, [&](const SceneEntity* innerNode){
		switch (quadrant){
		case Quadrant::X:{
			if ((!forward && innerNode->object->getPropFloat(attr_def::x) > node->object->getPropFloat(attr_def::x)) ||
				(forward && innerNode->object->getPropFloat(attr_def::x) < node->object->getPropFloat(attr_def::x)))
				return true;
			return false;
			}
			break;
		case Quadrant::Y:{
			if ((!forward && innerNode->object->getPropFloat(attr_def::y) > node->object->getPropFloat(attr_def::y)) ||
				(forward && innerNode->object->getPropFloat(attr_def::y) < node->object->getPropFloat(attr_def::y)))
				return true;
			return false;
			}
			break;
		case Quadrant::Z:{
			if ((!forward && innerNode->object->getPropFloat(attr_def::z) > node->object->getPropFloat(attr_def::z)) ||
				(forward && innerNode->object->getPropFloat(attr_def::z) < node->object->getPropFloat(attr_def::z)))
				return true;
			return false;
			}
			break;
		default: SLASSERT(false, "wtf"); return false; break;
		}
	}, forward);
}

bool Scene::isInVision(IObject* object, IObject* other){
	SLASSERT(object && other, "wtf");
	float otherX = other->getPropFloat(attr_def::x);
	float otherY = other->getPropFloat(attr_def::y);
	float objectX = object->getPropFloat(attr_def::x);
	float objectY = object->getPropFloat(attr_def::y);
	float vision = object->getPropFloat(attr_def::vision);
	if (abs(otherX - objectX) <= vision && abs(otherY - objectY) <= vision){
		return true;
	}
	return false;
}

void Scene::printSceneNodePos(){
	auto itor = _sceneObjectNodes.find("lxd");
	if (itor == _sceneObjectNodes.end())
		return;
	static int32 i = 0;
	i++;
	SceneEntity* curr = itor->second.xListHead.next;
	while (curr){
		//ECHO_ERROR("x[%d]:object[%lld:%d] pos[%f, %f, %f]", i, curr->object->getID(), curr->object->getPropInt32(attr_def::logic), curr->object->getPropFloat(attr_def::x), curr->object->getPropFloat(attr_def::y), curr->object->getPropFloat(attr_def::z));
		curr = curr->next;
	}

	curr = itor->second.yListHead.next;
	while (curr){
		//ECHO_ERROR("y[%d]:object[%lld:%d] pos[%f, %f, %f]", i,curr->object->getID(), curr->object->getPropInt32(attr_def::logic), curr->object->getPropFloat(attr_def::x), curr->object->getPropFloat(attr_def::y), curr->object->getPropFloat(attr_def::z));
		curr = curr->next;
	}
}
