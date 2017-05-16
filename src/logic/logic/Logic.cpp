#include "Logic.h"
#include "NodeProtocol.h"
#include "NodeDefine.h"
#include "IDCCenter.h"
#include "EventID.h"
#include "IEventEngine.h"
#include "IPlayerMgr.h"
#include "ProtocolID.pb.h"
#include "Protocol.pb.h"

bool Logic::initialize(sl::api::IKernel * pKernel){
	_self = this;
	return true;
}

bool Logic::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_objectMgr, ObjectMgr);
	FIND_MODULE(_playerMgr, PlayerMgr);
	FIND_MODULE(_eventEngine, EventEngine);

	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::GATE_MSG_BIND_PLAYER_REQ, Logic::onGateBindPlayerOnLogic);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::GATE_MSG_UNBIND_PLAYER_REQ, Logic::onGateUnBindPlayerOnLogic);
	RGS_NODE_HANDLER(_harbor, NodeProtocol::GATE_MSG_TRANSMIT_MSG_TO_LOGIC, Logic::onTransforMsgToLogic);


	return true;
}
bool Logic::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Logic::rgsProtocolHandler(int32 messageId, const HandleFunctionType& f, const char* debug){
	Handler handler{ NEW BProtocolHandler(f), debug };
	_protoHandlers[messageId].push_back(handler);
}

void Logic::onGateBindPlayerOnLogic(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int64 actorId = args.getInt64(0);
	int64 accountId = args.getInt64(1);

	bool success = _playerMgr->active(actorId, nodeId, accountId, [&](sl::api::IKernel* pKernel, IObject* object, bool isReconnect){
	});

	if (success){
		sendGateBindAck(pKernel, nodeId, accountId, actorId, protocol::ErrorCode::ERROR_NO_ERROR);
		sendSceneMgrBindNotify(pKernel, accountId, actorId, protocol::ErrorCode::ERROR_NO_ERROR);
		_gateActors[nodeId].insert(actorId);
	}
	else{
		sendGateBindAck(pKernel, nodeId, accountId, actorId, protocol::ErrorCode::ERROR_LOAD_PLAYER_FAILED);
	}
}

void Logic::onGateUnBindPlayerOnLogic(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int64 actorId = args.getInt64(0);
	_playerMgr->deActive(actorId, nodeId, false);
	_gateActors[nodeId].erase(actorId);
}

void Logic::onTransforMsgToLogic(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args){
	const void* context = args.getContext();
	const int32 size = args.getSize();
	int64 actorId = *(int64*)context;

	const char* dataBuf = (const char*)context + sizeof(int64);
	int32 messageId = *(int32*)dataBuf;
	int32 len = *((int32*)(dataBuf + sizeof(int32)));

	auto itor = _protoHandlers.find(messageId);
	if (itor == _protoHandlers.end() || itor->second.empty())
		return;

	IObject* object = _objectMgr->findObject(actorId);
	if (!object){
		SLASSERT(false, "not find object[%lld]", actorId);
		return;
	}

	for (auto handler : itor->second){
		handler._handler->dealProtocol(pKernel, object, args.getContext(), args.getSize());
	}
}

void Logic::onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId){
	if (nodeType == NodeType::GATE){
		for (auto actorId : _gateActors[nodeId]){
			_playerMgr->deActive(actorId, nodeId, false);
		}
		_gateActors[nodeId].clear();
	}
}

void Logic::sendGateBindAck(sl::api::IKernel* pKernel, int32 nodeId, int64 accountId, int64 actorId, int32 errorCode){
	IArgs<3, 32> args;
	args << errorCode << actorId << accountId;
	args.fix();

	_harbor->send(NodeType::GATE, nodeId, NodeProtocol::LOGIC_MSG_BIND_PLAYER_ACK, args.out());
}

void Logic::sendSceneMgrBindNotify(sl::api::IKernel* pKernel, int64 accountId, int64 actorId, int32 errorCode){
	IArgs<1, 32> notify;
	notify << actorId;
	notify.fix();
	_harbor->send(NodeType::SCENEMGR, 1, NodeProtocol::LOGIC_MSG_NOTIFY_ADD_PLAYER, notify.out());
}