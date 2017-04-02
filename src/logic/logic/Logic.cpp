#include "Logic.h"
#include "NodeProtocol.h"
#include "AgentProtocol.h"
#include "NodeDefine.h"
#include "IDCCenter.h"
#include "IRoleMgr.h"
#include "EventID.h"
#include "IEventEngine.h"

class RemoveObjectTimer : public sl::api::ITimer{
public:
	RemoveObjectTimer(IObject* object) :_objectId(object->getID()){}
	virtual ~RemoveObjectTimer() {}

	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onTerminate(sl::api::IKernel* pKernel, int64 timetick){
		Logic::getInstance()->recoverObject(pKernel, _objectId);
	}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}
private:
	int64 _objectId;
};

bool Logic::initialize(sl::api::IKernel * pKernel){
	_self = this;
	return true;
}

bool Logic::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_objectMgr, ObjectMgr);
	FIND_MODULE(_roleMgr, RoleMgr);
	FIND_MODULE(_eventEngine, EventEngine);

	RGS_NODE_HANDLER(_harbor, NodeProtocol::GATE_MSG_BIND_PLAYER_REQ, Logic::onGateBindPlayerOnLogic);
	RGS_NODE_HANDLER(_harbor, NodeProtocol::GATE_MSG_UNBIND_PLAYER_REQ, Logic::onGateUnBindPlayerOnLogic);

	return true;
}
bool Logic::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Logic::onGateBindPlayerOnLogic(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int64 actorId = args.getInt64(0);
	int64 accountId = args.getInt64(1);

	IObject* player = _objectMgr->findObject(actorId);
	if (player){
		SLASSERT(player->getPropInt32(attr_def::gate) == 0, "eyf");
		player->setPropInt32(attr_def::gate, nodeId);
		_gateActors[nodeId].insert(actorId);

		IArgs<3, 32> args;
		args << ProtocolError::ERROR_NO_ERROR << actorId << accountId;
		args.fix();

		_harbor->send(nodeType, nodeId, NodeProtocol::LOGIC_MSG_BIND_PLAYER_ACK, args.out());

		EventID::Biology info{ player };
		_eventEngine->execEvent(EventID::EVENT_GATE_RECONNECT, &info, sizeof(info));

		IArgs<1, 32> notify;
		notify << actorId;
		notify.fix();
		_harbor->send(NodeType::SCENEMGR, 1, NodeProtocol::LOGIC_MSG_NOTIFY_ADD_PLAYER, notify.out());

		SLASSERT(player->getPropInt64(attr_def::recoverTimer) != 0, "wtf");
		RemoveObjectTimer* recoverTimer = (RemoveObjectTimer*)player->getPropInt64(attr_def::recoverTimer);
		pKernel->killTimer(recoverTimer);
		player->setPropInt64(attr_def::recoverTimer, 0);
	}
	else{
		player = CREATE_OBJECT_BYID(_objectMgr, "Player", actorId);
		SLASSERT(player, "wtf");

		player->setPropInt64(attr_def::account, accountId);
		player->setPropInt32(attr_def::gate, nodeId);
		player->setPropInt32(attr_def::logic, _harbor->getNodeId());

		if (_roleMgr->loadRole(actorId, player)){
			_gateActors[nodeId].insert(actorId);

			IArgs<3, 32> args;
			args << ProtocolError::ERROR_NO_ERROR << actorId << accountId;
			args.fix();

			_harbor->send(nodeType, nodeId, NodeProtocol::LOGIC_MSG_BIND_PLAYER_ACK, args.out());

			EventID::Biology info{ player };
			_eventEngine->execEvent(EventID::EVENT_PLAYER_ONLINE, &info, sizeof(info));

			IArgs<1, 32> notify;
			notify << actorId;
			notify.fix();
			_harbor->send(NodeType::SCENEMGR, 1, NodeProtocol::LOGIC_MSG_NOTIFY_ADD_PLAYER, notify.out());
		}
		else{
			_objectMgr->recover(player);

			IArgs<3, 32> args;
			args << ProtocolError::ERROR_LOAD_PLAYER_FAILED << actorId << accountId;
			args.fix();

			_harbor->send(nodeType, nodeId, NodeProtocol::LOGIC_MSG_BIND_PLAYER_ACK, args.out());
		}
	}
	
}
void Logic::onGateUnBindPlayerOnLogic(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int64 actorId = args.getInt64(0);
	IObject* object = _objectMgr->findObject(actorId);
	SLASSERT(object, "wtf");
	if (object){
		SLASSERT(object->getPropInt32(attr_def::gate) == nodeId, "wtf");

		object->setPropInt32(attr_def::gate, 0);
		_gateActors[nodeId].erase(actorId);

		EventID::Biology info{ object };
		_eventEngine->execEvent(EventID::EVENT_GATE_LOST, &info, sizeof(info));

		SLASSERT(object->getPropInt64(attr_def::recoverTimer) == 0, "wtf");
		RemoveObjectTimer * recoverTimer = NEW RemoveObjectTimer(object);
		object->setPropInt64(attr_def::recoverTimer, (int64)recoverTimer);
		START_TIMER(recoverTimer, 0, 1, _recoverInterval);
	}
}

void Logic::recoverObject(sl::api::IKernel* pKernel, const int64 id){
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

void Logic::onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId){
	if (nodeType == NodeType::GATE){
		for (auto actorId : _gateActors[nodeId]){
			IObject* object = _objectMgr->findObject(actorId);
			SLASSERT(object, "wtf");
			if (object){
				if (object->getPropInt32(attr_def::gate) > 0){
					object->setPropInt32(attr_def::gate, 0);

					EventID::Biology info{ object };
					_eventEngine->execEvent(EventID::EVENT_GATE_LOST, &info, sizeof(info));

					SLASSERT(object->getPropInt64(attr_def::recoverTimer) == 0, "wtf");
					RemoveObjectTimer * recoverTimer = NEW RemoveObjectTimer(object);
					object->setPropInt64(attr_def::recoverTimer, (int64)recoverTimer);
					START_TIMER(recoverTimer, 0, 1, _recoverInterval);
				}
			}
		}
		_gateActors[nodeId].clear();
	}
}