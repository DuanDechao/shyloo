#include "SceneClientMgr.h"
#include "ILogic.h"
#include "ProtocolID.pb.h"
#include "IDCCenter.h"
#include "Attr.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "IEventEngine.h"
#include "EventID.h"
#include "slstring.h"
#include "GameDefine.h"
bool SceneClientMgr::initialize(sl::api::IKernel * pKernel){
	_self = this;
	return true;
}

bool SceneClientMgr::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	if (_harbor->getNodeType() != NodeType::LOGIC)
		return true;

	FIND_MODULE(_logic, Logic);
	FIND_MODULE(_eventEngine, EventEngine);

	RGS_PROTO_HANDLER(_logic, ClientMsgID::CLIENT_MSG_ENTER_SCENE_REQ, SceneClientMgr::onClientEnterScene);

	RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_LOGIC_DATA_LOAD_COMPLETED, SceneClientMgr::onPlayerDataLoadCompleted);

	return true;
}
bool SceneClientMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

bool SceneClientMgr::onClientEnterScene(sl::api::IKernel* pKernel, IObject* object, const sl::OBStream& args){
	
	if (object->getTempInt8(OCTempProp::IS_APPEAR) == 0){
		notifySceneMgrAppearScene(pKernel, object);

		logic_event::Biology evt;
		evt.object = object;
		_eventEngine->execEvent(logic_event::EVENT_LOGIC_PLAYER_APPEAR, &evt, sizeof(evt));
	}
	return true;
}

void SceneClientMgr::notifySceneMgrAppearScene(sl::api::IKernel* pKernel, IObject* object){
	if (object->getTempInt8(OCTempProp::IS_APPEAR) == 1)
		return;

	object->setTempInt8(OCTempProp::IS_APPEAR, 1);

	IArgs<2, 256> args;
	args << object->getID() << object->getPropInt32(attr_def::gate);
	args.fix();

	_harbor->send(NodeType::SCENEMGR, 1, NodeProtocol::LOGIC_MSG_NOTIFY_SCENEMGR_APPEAR_SCENE, args.out());
}

void SceneClientMgr::onPlayerDataLoadCompleted(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::Biology), "wtf");
	logic_event::Biology* info = (logic_event::Biology*)context;

	playerAppearOnScene(pKernel, info->object, true);
}

void SceneClientMgr::playerAppearOnScene(sl::api::IKernel* pKernel, IObject* object, bool distribute){
	SLASSERT(strcmp(object->getPropString(attr_def::scene), "") != 0, "invaild scene id");
	if (distribute){
		logic_event::Biology evt;
		evt.object = object;
		_eventEngine->execEvent(logic_event::EVENT_LOGIC_PREPARE_APPEAR_SCENE, &evt, sizeof(evt));
		
		sl::SLString<game::MAX_SCENE_LEN> sceneId;
		distributeScene(object->getPropString(attr_def::scene), sceneId);
		object->setPropString(attr_def::scene, sceneId.c_str());
	}

	notifySceneMgrEnterScene(pKernel, object);

	logic_event::Biology evt;
	evt.object = object;
	_eventEngine->execEvent(logic_event::EVENT_LOGIC_APPEAR_ON_SCENE, &evt, sizeof(evt));

	sendSceneInfo(pKernel, object);
}

bool SceneClientMgr::distributeScene(const char* scene, sl::SLString<game::MAX_SCENE_LEN>& sceneId){
	sceneId << scene;
	return true;
}

void SceneClientMgr::notifySceneMgrEnterScene(sl::api::IKernel* pKernel, IObject* object){
	logic_event::Biology evt;
	evt.object = object;
	_eventEngine->execEvent(logic_event::EVENT_LOGIC_PREPARE_ENTER_SCENE, &evt, sizeof(evt));
	
	IArgs<15, 1024> args;
	args << object->getID();
	args << object->getPropString(attr_def::scene);
	args << object->getPropFloat(attr_def::x);
	args << object->getPropFloat(attr_def::y);
	args << object->getPropFloat(attr_def::z);
	args << object->getPropInt32(attr_def::gate);
	args.fix();

	_harbor->send(NodeType::SCENEMGR, 1, NodeProtocol::LOGIC_MSG_NOTIFY_SCENEMGR_ENTER_SCENE, args.out());

	_eventEngine->execEvent(logic_event::EVENT_LOGIC_ENTER_SCENE, &evt, sizeof(evt));
}

void SceneClientMgr::notifySceneMgrLeaveScene(sl::api::IKernel* pKernel, IObject* object){
	logic_event::Biology evt;
	evt.object = object;
	_eventEngine->execEvent(logic_event::EVENT_LOGIC_PREPARE_LEAVE_SCENE, &evt, sizeof(evt));

	IArgs<15, 1024> args;
	args << object->getID();
	args.fix();

	_harbor->send(NodeType::SCENEMGR, 1, NodeProtocol::LOGIC_MSG_NOTIFY_SCENEMGR_LEAVE_SCENE, args.out());

	_eventEngine->execEvent(logic_event::EVENT_LOGIC_LEAVE_SCENE, &evt, sizeof(evt));
}

void SceneClientMgr::sendSceneInfo(sl::api::IKernel* pKernel, IObject* object){

}