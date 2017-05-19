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
#include "IObjectTimer.h"
#include "slxml_reader.h"
#include "Protocol.pb.h"

bool SceneClientMgr::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;

	sl::XmlReader svrConf;
	if (!svrConf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "cant load core file");
		return false;
	}
	_syncToSceneInterval = svrConf.root()["sync_rate"][0].getAttributeInt64("scene");

	return true;
}

bool SceneClientMgr::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	if (_harbor->getNodeType() != NodeType::LOGIC)
		return true;

	FIND_MODULE(_logic, Logic);
	FIND_MODULE(_eventEngine, EventEngine);
	FIND_MODULE(_objectTimer, ObjectTimer);

	RGS_PROTO_HANDLER(_logic, ClientMsgID::CLIENT_MSG_ENTER_SCENE_REQ, SceneClientMgr::onClientEnterScene);

	RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_LOGIC_DATA_LOAD_COMPLETED, SceneClientMgr::onPlayerDataLoadCompleted);
	RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_LOGIC_DATA_LOAD_COMPLETED, SceneClientMgr::onStartSyncTimer);
	RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_LOGIC_SCENE_OBJECT_APPEAR, SceneClientMgr::onObjectAppear);
	RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_LOGIC_SCENE_OBJECT_APPEAR, SceneClientMgr::onStartSyncTimer);
	RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_LOGIC_SCENE_OBJECT_DESTROY, SceneClientMgr::onObjectDisappear);
	RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_LOGIC_SCENE_OBJECT_DESTROY, SceneClientMgr::onStopSyncTimer);
	RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_LOGIC_PLAYER_DESTROY, SceneClientMgr::onPlayerDestroy);

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
	RGS_PROP_CHANGER(info->object, attr_def::z, SceneClientMgr::syncToScene);
}

void SceneClientMgr::syncToScene(sl::api::IKernel* pKernel, IObject* object, const char* name, const IProp* prop, const bool sync){
	object->setTempInt8(OCTempProp::SYNCTOSCENE, 1);
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

void SceneClientMgr::onStartSyncTimer(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::Biology), "wtf");
	IObject* object = ((logic_event::Biology*)context)->object;
	SLASSERT(object, "wtf");

	if (object->getTempInt64(OCTempProp::SYNCTOSCENE_TIMER) == 0)
		START_OBJECT_TIMER(_objectTimer, object, OCTempProp::SYNCTOSCENE_TIMER, 0, TIMER_BEAT_FOREVER, _syncToSceneInterval, SceneClientMgr::onSyncToSceneStart, SceneClientMgr::onSyncToSceneTime, SceneClientMgr::onSyncToSceneTerminate);
}

void SceneClientMgr::onStopSyncTimer(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::Biology), "wtf");
	IObject* object = ((logic_event::Biology*)context)->object;
	SLASSERT(object, "wtf");

	_objectTimer->stopTimer(object, OCTempProp::SYNCTOSCENE_TIMER);
}

void SceneClientMgr::onObjectAppear(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::Biology), "wtf");
	IObject* object = ((logic_event::Biology*)context)->object;
	
	logic_event::Biology evt;
	evt.object = object;
	_eventEngine->execEvent(logic_event::EVENT_LOGIC_PREPARE_APPEAR_SCENE, &evt, sizeof(evt));

	notifySceneMgrEnterScene(pKernel, object);
	notifySceneMgrAppearScene(pKernel, object);

	_eventEngine->execEvent(logic_event::EVENT_LOGIC_APPEAR_ON_SCENE, &evt, sizeof(evt));

	RGS_PROP_CHANGER(object, attr_def::z, SceneClientMgr::syncToScene);
}

void SceneClientMgr::onObjectDisappear(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::Biology), "wtf");
	IObject* object = ((logic_event::Biology*)context)->object;

	if (object->getPropInt8(attr_def::type) == protocol::ObjectType::OBJECT_TYPE_PLAYER)
		return;

	logic_event::Biology evt;
	evt.object = object;
	_eventEngine->execEvent(logic_event::EVENT_LOGIC_PREPARE_LEAVE_SCENE, &evt, sizeof(evt));

	notifySceneMgrLeaveScene(pKernel, object);

	_eventEngine->execEvent(logic_event::EVENT_LOGIC_LEAVE_SCENE, &evt, sizeof(evt));
}

void SceneClientMgr::onPlayerDestroy(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::Biology), "wtf");
	IObject* object = ((logic_event::Biology*)context)->object;
	SLASSERT(object, "wtf");

	logic_event::Biology evt;
	evt.object = object;
	_eventEngine->execEvent(logic_event::EVENT_LOGIC_PREPARE_LEAVE_SCENE, &evt, sizeof(evt));

	notifySceneMgrLeaveScene(pKernel, object);

	_eventEngine->execEvent(logic_event::EVENT_LOGIC_LEAVE_SCENE, &evt, sizeof(evt));
}

void SceneClientMgr::onSyncToSceneTime(sl::api::IKernel* pKernel, IObject* object, int64 tick){
	if (object->getTempInt8(OCTempProp::SYNCTOSCENE) != 1)
		return;

	float x = object->getPropFloat(attr_def::x);
	float y = object->getPropFloat(attr_def::y);
	float z = object->getPropFloat(attr_def::z);

	IArgs<10, 1024> args;
	args << object->getID() << x << y << z;
	args.fix();
	_harbor->prepareSend(NodeType::SCENEMGR, 1, NodeProtocol::LOGIC_MSG_SYNC_SCENE, args.getSize());
	_harbor->send(NodeType::SCENEMGR, 1, args.getContext(), args.getSize());

	object->setTempInt8(OCTempProp::SYNCTOSCENE, 0);
}

void SceneClientMgr::onSyncToSceneTerminate(sl::api::IKernel* pKernel, IObject* object, bool, int64){
	object->setTempInt8(OCTempProp::SYNCTOSCENE, 0);
}
