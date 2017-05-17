#include "PlayerMgr.h"
#include "IHarbor.h"
#include "IDCCenter.h"
#include "EventID.h"
#include "IEventEngine.h"
#include "IRoleMgr.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "Attr.h"
#include "ICacheDB.h"
#include "OCTimer.h"
#include "slxml_reader.h"
#include "slbinary_map.h"

class RemoveObjectTimer : public sl::api::ITimer{
public:
	RemoveObjectTimer(IObject* object) :_objectId(object->getID()){}
	virtual ~RemoveObjectTimer() {}

	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onTerminate(sl::api::IKernel* pKernel, int64 timetick){
		PlayerMgr::getInstance()->recoverObject(pKernel, _objectId);
	}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}
private:
	int64 _objectId;
};

PlayerMgr* PlayerMgr::s_self = nullptr;
bool PlayerMgr::initialize(sl::api::IKernel * pKernel){
	s_self = this;
	_kernel = pKernel;

	sl::XmlReader svrConf;
	if (!svrConf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "cant load core file");
		return false;
	}
	_savePlayerInterval = svrConf.root()["save"][0].getAttributeInt64("player");
	
	return true;
}

bool PlayerMgr::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_objectMgr, ObjectMgr);
	FIND_MODULE(_eventEngine, EventEngine);
	FIND_MODULE(_roleMgr, RoleMgr);
	FIND_MODULE(_cacheDB, CacheDB);

	return true;
}

bool PlayerMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

bool PlayerMgr::active(int64 actorId, int32 nodeId, int64 accountId, const std::function<void(sl::api::IKernel* pKernel, IObject* object, bool isReconnect)>& f){
	IObject* player = _objectMgr->findObject(actorId);
	if (player){
		SLASSERT(player->getPropInt32(attr_def::gate) == 0, "eyf");
		player->setPropInt32(attr_def::gate, nodeId);

		f(_kernel, player, true);

		logic_event::Biology info{ player };
		_eventEngine->execEvent(logic_event::EVENT_GATE_RECONNECT, &info, sizeof(info));

		SLASSERT(player->getPropInt64(attr_def::recoverTimer) != 0, "wtf");
		RemoveObjectTimer* recoverTimer = (RemoveObjectTimer*)player->getPropInt64(attr_def::recoverTimer);
		_kernel->killTimer(recoverTimer);
		player->setPropInt64(attr_def::recoverTimer, 0);
	}
	else{
		player = CREATE_OBJECT_BYID(_objectMgr, "Player", actorId);
		SLASSERT(player, "wtf");

		player->setPropInt64(attr_def::account, accountId);
		player->setPropInt32(attr_def::gate, nodeId);
		player->setPropInt32(attr_def::logic, _harbor->getNodeId());

		if (_roleMgr->loadRole(actorId, player)){
			f(_kernel, player, false);
		
			onProcessPlayerOnline(_kernel, player);
		}
		else{
			_objectMgr->recover(player);
			return false;
		}
	}

	return true;
}

void PlayerMgr::onProcessPlayerOnline(sl::api::IKernel* pKernel, IObject* object){
	logic_event::Biology info{ object };
	_eventEngine->execEvent(logic_event::EVENT_PLAYER_ONLINE, &info, sizeof(info));

	allDataLoadComplete(pKernel, object);
}

void PlayerMgr::allDataLoadComplete(sl::api::IKernel* pKernel, IObject* object){

	//TODO 向客户端首次同步属性

	logic_event::Biology info{ object };
	_eventEngine->execEvent(logic_event::EVENT_DATA_LOAD_COMPLETED, &info, sizeof(info));

	RGS_PROP_CHANGER(object, ANY_CALL, PlayerMgr::propSync);
}

void PlayerMgr::propSync(sl::api::IKernel* pKernel, IObject* object, const char* name, const IProp* prop, const bool sync){
	int32 setting = prop->getSetting(object);
	if (setting & prop_def::save){
		if (setting & prop_def::significant){
			savePlayer(pKernel, object);
		}
		else{
			if (object->getTempInt64(OCTempProp::PROP_UPDATE_TIMER) == 0){
				OCTimer* timer = OCTimer::create(pKernel, object, OCTempProp::PROP_UPDATE_TIMER, nullptr, PlayerMgr::onSavePlayerTime, PlayerMgr::onSavePlayerTerminate);
				object->setTempInt64(OCTempProp::PROP_UPDATE_TIMER, (int64)timer);
				START_TIMER(timer, 0, 1, _savePlayerInterval);
			}
		}
	}
}

void PlayerMgr::onSavePlayerTime(sl::api::IKernel* pKernel, IObject* object, int64 tick){
	s_self->savePlayer(pKernel, object);
}

void PlayerMgr::onSavePlayerTerminate(sl::api::IKernel* pKernel, IObject* object, bool, int64){
	OCTimer* timer = (OCTimer*)object->getTempInt64(OCTempProp::PROP_UPDATE_TIMER);
	if (timer)
		timer->release();
	object->setTempInt64(OCTempProp::PROP_UPDATE_TIMER, 0);
}

bool PlayerMgr::savePlayer(sl::api::IKernel* pKernel, IObject* player){
	bool ret = _cacheDB->writeByIndex("actor", [&](sl::api::IKernel* pKernel, ICacheDBContext* context){
		context->writeInt64("id", player->getID());
		context->writeString("name", player->getPropString(attr_def::name));
		context->writeInt8("occupation", player->getPropInt8(attr_def::occupation));
		context->writeInt8("sex", player->getPropInt8(attr_def::sex));

		sl::IBMap<4096, 4096> props;
		for (const IProp* prop : player->getObjProps()){
			int32 setting = prop->getSetting(player);
			if ((setting & prop_def::save) && (setting & prop_def::blob)){
				switch (prop->getType(player)){
				case DTYPE_INT8: props.writeInt8(prop->getName(), player->getPropInt8(prop)); break;
				case DTYPE_INT16: props.writeInt16(prop->getName(), player->getPropInt16(prop)); break;
				case DTYPE_INT32: props.writeInt32(prop->getName(), player->getPropInt32(prop)); break;
				case DTYPE_INT64: props.writeInt64(prop->getName(), player->getPropInt64(prop)); break;
				case DTYPE_FLOAT: props.writeFloat(prop->getName(), player->getPropFloat(prop)); break;
				case DTYPE_STRING: props.writeString(prop->getName(), player->getPropString(prop)); break;
				case DTYPE_BLOB:{
						int32 size = 0;
						const void* p = player->getPropBlob(prop, size);
						if (size > 0)
							props.writeBlob(prop->getName(), p, size);
					}
					break;
				}
			}
		}
		props.fix();
		context->writeBlob("props", props.getContext(), props.getSize());
	}, player->getPropInt64(attr_def::account));

	return ret;
}

bool PlayerMgr::deActive(int64 actorId, int32 nodeId, bool isPlayerOpt){
	IObject* object = _objectMgr->findObject(actorId);
	SLASSERT(object, "wtf");
	if (object){
		if (object->getPropInt32(attr_def::gate) != nodeId){
			SLASSERT(false, "invaild gate node id");
			return false;
		}

		object->setPropInt32(attr_def::gate, game::INVAILD_GATE_NODE_ID);

		logic_event::Biology info{ object };
		_eventEngine->execEvent(logic_event::EVENT_GATE_LOST, &info, sizeof(info));

		SLASSERT(object->getPropInt64(attr_def::recoverTimer) == 0, "wtf");
		RemoveObjectTimer * recoverTimer = NEW RemoveObjectTimer(object);
		object->setPropInt64(attr_def::recoverTimer, (int64)recoverTimer);
		sl::api::IKernel* pKernel = _kernel;
		START_TIMER(recoverTimer, 0, 1, _recoverInterval);
	}
	else{
		SLASSERT(false, "can not find object %lld", actorId);
		return false;
	}
	return true;
}

void PlayerMgr::recoverObject(sl::api::IKernel* pKernel, const int64 id){
	IObject* object = _objectMgr->findObject(id);
	SLASSERT(object, "wtd");
	if (object){
		object->setPropInt64(attr_def::recoverTimer, 0);

		logic_event::Biology info{ object };
		_eventEngine->execEvent(logic_event::EVENT_PLAYER_DESTROY, &info, sizeof(info));

		_roleMgr->recoverPlayer(object);
		_objectMgr->recover(object);

		IArgs<1, 32> notify;
		notify << id;
		notify.fix();
		_harbor->send(NodeType::SCENEMGR, 1, NodeProtocol::LOGIC_MSG_NOTIFY_REMOVE_PLAYER, notify.out());
	}
}

