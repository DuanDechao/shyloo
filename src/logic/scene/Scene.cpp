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
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::SCENEMGR_MSG_LEAVE_SCENE, Scene::onSceneMgrAppearScene);

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
	sceneUnit->setPropInt32(attr_def::gate, gate);

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
	SLASSERT(object && strcmp(object->getPropString(attr_def::sceneId), sceneObj->getPropString(attr_def::id)) == 0, "witf");
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

	SLASSERT(object->getTempInt8(OCTempProp::IS_APPEAR) == 0, "wtf");
	if (object->getTempInt8(OCTempProp::IS_APPEAR) == 1)
		return;

	object->setTempInt8(OCTempProp::IS_APPEAR, 1);
	object->setPropInt32(attr_def::gate, gate);

	logic_event::AppearVision evt;
	evt.object = object;
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

	SLASSERT(sceneObject->getTempInt8(OCTempProp::IS_APPEAR) == 1, "wtf");
	if (sceneObject->getTempInt8(OCTempProp::IS_APPEAR) == 1){
	}

	DEL_TABLE_ROW(sceneObjectsTab, findRow);

	logic_event::LeaveVision evt;
	evt.object = sceneObj;
	evt.id = id;
	_eventEngine->execEvent(logic_event::EVENT_SCENE_LEAVE_SCENE, &evt, sizeof(evt));

	_objectMgr->recover(sceneObject);

	_capacityPublisher->decreaseLoad(1);
}

IObject* Scene::findScene(const char* scene){
	const IRow* row = _scenes->findRow(scene);
	if (!row)
		return nullptr;

	return (IObject*)row->getDataInt64(OCStaticTableMacro::SCENECOPY::OBJECT);
}
