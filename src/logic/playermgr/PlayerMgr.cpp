#include "PlayerMgr.h"
#include "IHarbor.h"
#include "IDCCenter.h"
#include "EventID.h"
#include "IEventEngine.h"
#include "IRoleMgr.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"

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

bool PlayerMgr::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	return true;
}

bool PlayerMgr::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_objectMgr, ObjectMgr);
	FIND_MODULE(_eventEngine, EventEngine);
	FIND_MODULE(_roleMgr, RoleMgr);
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

		EventID::Biology info{ player };
		_eventEngine->execEvent(EventID::EVENT_GATE_RECONNECT, &info, sizeof(info));

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
		
			EventID::Biology info{ player };
			_eventEngine->execEvent(EventID::EVENT_PLAYER_ONLINE, &info, sizeof(info));
		}
		else{
			_objectMgr->recover(player);
			return false;
		}
	}

	return true;
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

		EventID::Biology info{ object };
		_eventEngine->execEvent(EventID::EVENT_GATE_LOST, &info, sizeof(info));

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

		EventID::Biology info{ object };
		_eventEngine->execEvent(EventID::EVENT_PLAYER_DESTROY, &info, sizeof(info));

		_roleMgr->recoverPlayer(object);
		_objectMgr->recover(object);

		IArgs<1, 32> notify;
		notify << id;
		notify.fix();
		_harbor->send(NodeType::SCENEMGR, 1, NodeProtocol::LOGIC_MSG_NOTIFY_REMOVE_PLAYER, notify.out());
	}
}

