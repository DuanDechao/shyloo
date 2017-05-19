#include "SceneMgr.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "IDCCenter.h"
#include "ICapacity.h"
bool SceneMgr::initialize(sl::api::IKernel * pKernel){
	_self = this;
	return true;
}

bool SceneMgr::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);

	if (_harbor->getNodeType() != NodeType::SCENEMGR)
		return true;

	FIND_MODULE(_objectMgr, ObjectMgr);
	FIND_MODULE(_capacity, CapacitySubscriber);

	_objects = CREATE_STATIC_TABLE(_objectMgr, OCStaticTableMacro::STATICSCENEOBJECTS::TABLE_NAME, OCStaticTableMacro::STATICSCENEOBJECTS::TABLE_NAME);
	_scenes = CREATE_STATIC_TABLE(_objectMgr, OCStaticTableMacro::SCENES::TABLE_NAME, OCStaticTableMacro::SCENES::TABLE_NAME);

	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::LOGIC_MSG_NOTIFY_SCENEMGR_ENTER_SCENE, SceneMgr::onLogicEnterScene);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::LOGIC_MSG_NOTIFY_SCENEMGR_APPEAR_SCENE, SceneMgr::onLogicAppearScene);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::LOGIC_MSG_NOTIFY_SCENEMGR_LEAVE_SCENE, SceneMgr::onLogicLeaveScene);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::LOGIC_MSG_SYNC_SCENE, SceneMgr::onLogicUpdatePosition);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::SCENE_MSG_SCENE_CONFIRMED, SceneMgr::onSceneConfirmScene);
	

	return true;
}

bool SceneMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void SceneMgr::onLogicEnterScene(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int64 id = args.getInt64(0);
	const char* scene = args.getString(1);
	float x = args.getFloat(2);
	float y = args.getFloat(3);
	float z = args.getFloat(4);
	int32 gate = args.getInt32(5);

	distributeScene(pKernel, scene);

#ifdef _DEBUG
	{
		const IRow* row = _objects->findRow(id);
		SLASSERT(!row, "wtf");
	}
#endif // DEBUG

	const IRow* row = _objects->addRowKeyInt64(id);
	row->setDataString(OCStaticTableMacro::STATICSCENEOBJECTS::SCENEID, scene);

	addObjectToScene(pKernel, scene, id, x, y, z, gate);
}

void SceneMgr::distributeScene(sl::api::IKernel* pKernel, const char* scene){
	int32 nodeId = game::NODE_INVALID_ID;
	const IRow* row = _scenes->findRow(scene);
	if (!row){
		row = _scenes->addRowKeyString(scene);
		SLASSERT(row, "wtf");

		nodeId = _capacity->choose(NodeType::SCENE);
		row->setDataString(OCStaticTableMacro::SCENES::NAME, scene);
		row->setDataInt32(OCStaticTableMacro::SCENES::NODE, nodeId);
		row->setDataInt32(OCStaticTableMacro::SCENES::STATE, SceneState::SS_DISTRIBUTE);
	}
	else{
		row->setDataInt32(OCStaticTableMacro::SCENES::STATE, SceneState::SS_DISTRIBUTE);
		nodeId = row->getDataInt32(OCStaticTableMacro::SCENES::NODE);
	}

	IArgs<7, 1024> args;
	args << scene;
	args.fix();
	_harbor->send(NodeType::SCENE, nodeId, NodeProtocol::SCENEMGR_MSG_CREATE_SCENE, args.out());
}

void SceneMgr::addObjectToScene(sl::api::IKernel* pKernel, const char* scene, int64 id, float x, float y, float z, int32 gate){
	const IRow* row = _scenes->findRow(scene);
	SLASSERT(row, "wtf");

	const char* name = row->getDataString(OCStaticTableMacro::SCENES::NAME);
	int32 nodeId = row->getDataInt32(OCStaticTableMacro::SCENES::NODE);
	int32 count = row->getDataInt32(OCStaticTableMacro::SCENES::COUNT);

	IArgs<15, 1024> args;
	args << scene << id << x << y << z << gate;
	args.fix();
	_harbor->send(NodeType::SCENE, nodeId, NodeProtocol::SCENEMGR_MSG_ENTER_SCENE, args.out());

	row->setDataInt32(OCStaticTableMacro::SCENES::COUNT, count + 1);
}

void SceneMgr::onSceneConfirmScene(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	const char* sceneId = args.getString(0);

	const IRow* row = _scenes->findRow(sceneId);
	SLASSERT(row, "wtf");
	if (row){
		SLASSERT(row->getDataInt32(OCStaticTableMacro::SCENES::NODE) == nodeId, "confirm scene but nodeid is not correct");
		row->setDataInt32(OCStaticTableMacro::SCENES::STATE, SceneState::SS_CONFIRMED);
	}
}

void SceneMgr::onLogicAppearScene(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int64 id = args.getInt64(0);
	int32 gate = args.getInt32(1);

	const IRow* row = _objects->findRow(id);
	if (!row){
		SLASSERT(false, "wtf");
		return;
	}

	const char* sceneId = row->getDataString(OCStaticTableMacro::STATICSCENEOBJECTS::SCENEID);
	appearObjectToScene(pKernel, sceneId, id, gate);
}

void SceneMgr::onLogicLeaveScene(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int64 id = args.getInt64(0);

	const IRow* row = _objects->findRow(id);
	if (!row){
		SLASSERT(false, "wtf");
		return;
	}

	const char* sceneId = row->getDataString(OCStaticTableMacro::STATICSCENEOBJECTS::SCENEID);
	
	removeObjectFromScene(pKernel, sceneId, id);
	
	DEL_TABLE_ROW(_objects, row);
}

void SceneMgr::appearObjectToScene(sl::api::IKernel* pKernel, const char* scene, int32 id, int32 gate){
	const IRow* row = _scenes->findRow(scene);
	SLASSERT(row, "wtf");

	int32 nodeId = row->getDataInt32(OCStaticTableMacro::SCENES::NODE);

	IArgs<10, 1024> args;
	args << scene << id << gate;
	args.fix();
	_harbor->send(NodeType::SCENE, nodeId, NodeProtocol::SCENEMGR_MSG_APPEAR_SCENE, args.out());
}

void SceneMgr::removeObjectFromScene(sl::api::IKernel* pKernel, const char* scene, int32 id){
	const IRow* row = _scenes->findRow(scene);
	SLASSERT(row, "wtf");

	const char* sceneName = row->getDataString(OCStaticTableMacro::SCENES::NAME);
	int32 nodeId = row->getDataInt32(OCStaticTableMacro::SCENES::NODE);
	int32 count = row->getDataInt32(OCStaticTableMacro::SCENES::COUNT);

	IArgs<10, 1024> args;
	args << scene << id;
	args.fix();
	_harbor->send(NodeType::SCENE, nodeId, NodeProtocol::SCENEMGR_MSG_LEAVE_SCENE, args.out());

	row->setDataInt32(OCStaticTableMacro::SCENES::COUNT, count - 1);
}

void SceneMgr::onLogicUpdatePosition(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int64 id = args.getInt64(0);
	float x = args.getFloat(1);
	float y = args.getFloat(2);
	float z = args.getFloat(3);

	const IRow* row = _objects->findRow(id);
	if (!row){
		SLASSERT(false, "wtf");
		return;
	}

	const char* sceneId = row->getDataString(OCStaticTableMacro::STATICSCENEOBJECTS::SCENEID);
	const IRow* sceneRow = _scenes->findRow(sceneId);
	if (!sceneRow){
		SLASSERT(false, "wtf");
		return;
	}

	int32 node = sceneRow->getDataInt32(OCStaticTableMacro::SCENES::NODE);
	IArgs<7, 1024> inArgs;
	inArgs << sceneId << id << x << y << z;
	inArgs.fix();
	_harbor->send(NodeType::SCENE, node, NodeProtocol::SCENEMGR_MSG_SYNC_SCENE, inArgs.out());
}