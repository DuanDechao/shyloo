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
#include "IObjectTimer.h"
#include "slxml_reader.h"
#include "slbinary_map.h"
#include "ILogic.h"
#include "ProtocolID.pb.h"
#include "IPropDelaySender.h"
#include "Protocol.pb.h"

#define FOREACH_DELAY 100

class RemoveObjectTimer : public sl::api::ITimer{
public:
	RemoveObjectTimer(IObject* object) :_objectId(object->getID()){}
	virtual ~RemoveObjectTimer() {}

	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){
		PlayerMgr::getInstance()->recoverObject(pKernel, _objectId);
	}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}
private:
	int64 _objectId;
};

IEventEngine* PlayerMgr::s_eventEngine = nullptr;
bool PlayerMgr::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;

	sl::XmlReader svrConf;
	if (!svrConf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "cant load core file");
		return false;
	}
	_savePlayerInterval = svrConf.root()["player_mgr"][0].getAttributeInt64("save");
	_recoverInterval = svrConf.root()["player_mgr"][0].getAttributeInt64("recover");
	_foreachCount = svrConf.root()["player_mgr"][0].getAttributeInt64("foreach_count");
	
	return true;
}

bool PlayerMgr::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_objectMgr, ObjectMgr);
	FIND_MODULE(s_eventEngine, EventEngine);
	FIND_MODULE(_roleMgr, RoleMgr);
	FIND_MODULE(_cacheDB, CacheDB);
	FIND_MODULE(_objectTimer, ObjectTimer);
	FIND_MODULE(_logic, Logic);
	FIND_MODULE(_propDelaySender, PropDelaySender);

	RGS_PROTO_HANDLER(_logic, ClientMsgID::CLIENT_MSG_TEST, PlayerMgr::onClientTestReq);

	RGS_EVENT_HANDLER(s_eventEngine, logic_event::EVENT_NEW_DAY, PlayerMgr::OnNewDayComing);
	RGS_EVENT_HANDLER(s_eventEngine, logic_event::EVENT_NEW_WEEK, PlayerMgr::OnNewWeekComing);
	RGS_EVENT_HANDLER(s_eventEngine, logic_event::EVENT_NEW_MONTH, PlayerMgr::OnNewMonthComing);

	return true;
}

bool PlayerMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

IObject* PlayerMgr::findPlayer(const char* name){
	auto itor = _onlines.find(name);
	if (itor == _onlines.end())
		return nullptr;

	return findPlayer(itor->second);
}

IObject* PlayerMgr::findPlayer(const int64 id){
	IObject* player = _objectMgr->findObject(id);
	if (player == nullptr)
		return nullptr;

	if (player->getPropInt32(attr_def::gate) == game::INVAILD_GATE_NODE_ID)
		return nullptr;

	return player;
}

void PlayerMgr::save(IObject* object){
	startSavePlayerTimer(_kernel, object);
}

bool PlayerMgr::active(int64 actorId, int32 nodeId, int64 accountId, const std::function<void(sl::api::IKernel* pKernel, IObject* object, bool isReconnect)>& f){
	IObject* player = _objectMgr->findObject(actorId);
	if (player){
		SLASSERT(player->getPropInt32(attr_def::gate) == 0, "eyf");
		player->setPropInt32(attr_def::gate, nodeId);

		f(_kernel, player, true);

		logic_event::Biology info{ player };
		s_eventEngine->execEvent(logic_event::EVENT_LOGIC_PLAYER_RECONNECT, &info, sizeof(info));

		SLASSERT(player->getPropInt64(attr_def::recoverTimer) != 0, "wtf");
		RemoveObjectTimer* recoverTimer = (RemoveObjectTimer*)player->getPropInt64(attr_def::recoverTimer);
		_kernel->killTimer(recoverTimer);
		player->setPropInt64(attr_def::recoverTimer, 0);
	}
	else{
		player = CREATE_OBJECT_BYID(_objectMgr, "Player", actorId);
		SLASSERT(player, "wtf");

		player->setPropInt8(attr_def::type, protocol::ObjectType::OBJECT_TYPE_PLAYER);
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

	const char* name = object->getPropString(attr_def::name);
	SLASSERT(_onlines.find(name) == _onlines.end(), "player[%lld] is already online", object->getID());
	_onlines[name] = object->getID();


	if (object->getPropInt8(attr_def::firstLogin) == 1){
		s_eventEngine->execEvent(logic_event::EVENT_LOGIC_PLAYER_FIRST_ONLINE, &info, sizeof(info));
		object->setPropInt8(attr_def::firstLogin, 0);

		_self->savePlayer(pKernel, object);
	}

	s_eventEngine->execEvent(logic_event::EVENT_LOGIC_PLAYER_ONLINE, &info, sizeof(info));

	allDataLoadComplete(pKernel, object);
}

void PlayerMgr::allDataLoadComplete(sl::api::IKernel* pKernel, IObject* object){

	//TODO 向客户端首次同步属性

	logic_event::Biology info{ object };
	s_eventEngine->execEvent(logic_event::EVENT_LOGIC_DATA_LOAD_COMPLETED, &info, sizeof(info));

	RGS_PROP_CHANGER(object, ANY_CALL, PlayerMgr::propSync);
}

void PlayerMgr::startSavePlayerTimer(sl::api::IKernel* pKernel, IObject* object){
	if (object->getTempInt64(OCTempProp::PROP_UPDATE_TIMER) == 0){
		START_OBJECT_TIMER(_objectTimer, object, OCTempProp::PROP_UPDATE_TIMER, 0, 1, _savePlayerInterval, PlayerMgr::onSavePlayerStart, PlayerMgr::onSavePlayerTime, PlayerMgr::onSavePlayerTerminate);
	}
}

void PlayerMgr::propSync(sl::api::IKernel* pKernel, IObject* object, const char* name, const IProp* prop, const bool sync){
	int32 setting = prop->getSetting(object);
	if (setting & prop_def::persistent){
		if (setting & prop_def::persistent){
			savePlayer(pKernel, object);
		}
		else{
			startSavePlayerTimer(pKernel, object);
		}
	}
}

void PlayerMgr::onSavePlayerTime(sl::api::IKernel* pKernel, IObject* object, int64 tick){
	_self->savePlayer(pKernel, object);
}

bool PlayerMgr::savePlayer(sl::api::IKernel* pKernel, IObject* player){
	bool ret = _cacheDB->write("actor", false, [&](sl::api::IKernel* pKernel, ICacheDBContext* context){
		//context->writeInt64("id", player->getID());
		context->writeString("name", player->getPropString(attr_def::name));
		context->writeInt8("occupation", player->getPropInt8(attr_def::occupation));
		context->writeInt8("sex", player->getPropInt8(attr_def::sex));

		sl::BMap<4096, 4096> props;
		for (const IProp* prop : player->getObjProps()){
			int32 setting = prop->getSetting(player);
			if ((setting & prop_def::persistent) && (setting & prop_def::persistent)){
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
	}, 1, player->getID());

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
		s_eventEngine->execEvent(logic_event::EVENT_LOGIC_PLAYER_LOST, &info, sizeof(info));

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
	if (!object)
		return;

	if (object->getPropInt32(attr_def::gate) == game::INVAILD_GATE_NODE_ID)
		return;

	object->setPropInt32(attr_def::gate, game::INVAILD_GATE_NODE_ID);
	object->setPropInt64(attr_def::recoverTimer, 0);

	logic_event::Biology info{ object };
	s_eventEngine->execEvent(logic_event::EVENT_LOGIC_PLAYER_GATE_LOST, &info, sizeof(info));

	object->setPropInt64(attr_def::offlineTime, sl::getTimeMilliSecond());

	if (strcmp(object->getPropString(attr_def::name), "") != 0){
		_onlines.erase(object->getPropString(attr_def::name));
	}

	if (object->getTempInt64(OCTempProp::PROP_UPDATE_TIMER)){
		savePlayer(pKernel, object);
		_objectTimer->stopTimer(object, OCTempProp::PROP_UPDATE_TIMER);
	}

	s_eventEngine->execEvent(logic_event::EVENT_LOGIC_PLAYER_DESTROY, &info, sizeof(info));
	_roleMgr->recoverPlayer(object);
	_objectMgr->recover(object);
}

void PlayerMgr::foreach(const IProp* prop, const std::function<void(sl::api::IKernel* pKernel, IObject* object, int64 tick)>& f){
	sl::api::IKernel* pKernel = _kernel;
	int32 idx = 0;
	if (_onlines.empty())
		return;

	for (auto itor = _onlines.begin(); itor != _onlines.end(); ++itor){
		IObject* player = _objectMgr->findObject(itor->second);
		SLASSERT(player, "where is online player[%lld]", itor->second);
		if (player){
			_objectTimer->stopTimer(player, prop);

			_objectTimer->startTimer(player, prop, 0, 1, (idx++ / _foreachCount) * FOREACH_DELAY, nullptr, f, nullptr, __FILE__, __LINE__);
		}
	}
}

void PlayerMgr::OnNewDayComing(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(sizeof(logic_event::NewDay) == size, "wtf");
	_self->foreach(OCTempProp::DAY_CHANGE_TIMER, PlayerMgr::OnDayChange);
}

void PlayerMgr::OnNewWeekComing(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(sizeof(logic_event::NewWeek) == size, "wtf");
	_self->foreach(OCTempProp::WEEK_CHANGE_TIMER, PlayerMgr::OnWeekChange);
}

void PlayerMgr::OnNewMonthComing(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(sizeof(logic_event::NewMonth) == size, "wtf");
	_self->foreach(OCTempProp::MONTH_CHANGE_TIMER, PlayerMgr::OnMonthChange);
}

void PlayerMgr::OnDayChange(sl::api::IKernel* pKernel, IObject* object, int64 tick){
	logic_event::Biology evt{ object };
	s_eventEngine->execEvent(logic_event::EVENT_DAY_CHANGED, &evt, sizeof(evt));
	ECHO_TRACE("player new day event.....");
}

void PlayerMgr::OnWeekChange(sl::api::IKernel* pKernel, IObject* object, int64 tick){
	logic_event::Biology evt{ object };
	s_eventEngine->execEvent(logic_event::EVENT_WEEK_CHANGED, &evt, sizeof(evt));
}

void PlayerMgr::OnMonthChange(sl::api::IKernel* pKernel, IObject* object, int64 tick){
	logic_event::Biology evt{ object };
	s_eventEngine->execEvent(logic_event::EVENT_MONTH_CHANGED, &evt, sizeof(evt));
}

bool PlayerMgr::onClientTestReq(sl::api::IKernel* pKernel, IObject* object, const sl::OBStream& args){
	object->setPropString(attr_def::name, "ddc");
	_propDelaySender->syncChangedProps(object);
	return true;
}
