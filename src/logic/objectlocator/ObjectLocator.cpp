#include "ObjectLocator.h"
#include "IEventEngine.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "EventID.h"
#include "IDCCenter.h"
#include "NodeProtocol.h"
#include "GameDefine.h"

bool ObjectLocator::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	_self = this;
	return true;
}

bool ObjectLocator::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	if (_harbor->getNodeType() == NodeType::LOGIC){
		FIND_MODULE(_eventEngine, EventEngine);

		RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_LOGIC_PLAYER_ONLINE, ObjectLocator::onSyncLocation);
		RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_LOGIC_SCENE_OBJECT_APPEAR, ObjectLocator::onSyncLocation);
		RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_LOGIC_SCENE_OBJECT_DESTROY, ObjectLocator::onRemoveLocation);
	}

	if (_harbor->getNodeType() == NodeType::RELATION){
		RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::LOGIC_MSG_SYNC_OBJECT_LOCATION, ObjectLocator::onLogicSyncObjectLocation);
		RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::LOGIC_MSG_REMOVE_OBJECT_LOCATION, ObjectLocator::onLogicRemoveObjectLocation);
	}
	return true;
}

bool ObjectLocator::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

int32 ObjectLocator::findObjectGate(int64 id){
	auto itor = _allObjects.find(id);
	if (itor == _allObjects.end())
		return itor->second._gate;

	return game::NODE_INVALID_ID;
}

int32 ObjectLocator::findObjectLogic(int64 id){
	auto itor = _allObjects.find(id);
	if (itor == _allObjects.end())
		return itor->second._logic;

	return game::NODE_INVALID_ID;
}

void ObjectLocator::onSyncLocation(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::Biology), "wtf");
	IObject* object = ((logic_event::Biology*)context)->object;
	SLASSERT(object, "wtf");

	IArgs<2, 64> args;
	args << object->getID() << object->getPropInt32(attr_def::gate);
	args.fix();
	_harbor->send(NodeType::RELATION, 1, NodeProtocol::LOGIC_MSG_SYNC_OBJECT_LOCATION, args.out());
}

void ObjectLocator::onRemoveLocation(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::Biology), "wtf");
	IObject* object = ((logic_event::Biology*)context)->object;
	SLASSERT(object, "wtf");

	IArgs<2, 64> args;
	args << object->getID();
	args.fix();
	_harbor->send(NodeType::RELATION, 1, NodeProtocol::LOGIC_MSG_REMOVE_OBJECT_LOCATION, args.out());
}

void ObjectLocator::onLogicSyncObjectLocation(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int64 id = args.getInt64(0);
	int32 gate = args.getInt32(1);

	SLASSERT(_allObjects.find(id) == _allObjects.end(), "wtf");
	_allObjects[id] = { nodeId, gate };
}

void ObjectLocator::onLogicRemoveObjectLocation(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int64 id = args.getInt64(0);

	SLASSERT(_allObjects.find(id) != _allObjects.end(), "wtf");
	_allObjects.erase(id);
}





