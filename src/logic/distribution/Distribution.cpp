#include "Distribution.h"
#include "IHarbor.h"
#include "ICapacity.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"

bool Distribution::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	return true;
}

bool Distribution::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	if (_harbor->getNodeType() == NodeType::SCENEMGR){
		FIND_MODULE(_capacity, CapacitySubscriber);

		RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::GATE_MSG_DISTRIBUTE_LOGIC_REQ, Distribution::onGateDistributeLogic);
		RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::LOGIC_MSG_NOTIFY_ADD_PLAYER, Distribution::onLogicAddPlayer);
		RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::LOGIC_MSG_NOTIFY_REMOVE_PLAYER, Distribution::onLogicRemovePlayer);
	}
	return true;
}

bool Distribution::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Distribution::onGateDistributeLogic(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int64 agentId = args.getInt64(0);
	int64 actorId = args.getInt64(1);
	int32 logic = 0;

	auto itor = _distributes.find(actorId);
	if (itor != _distributes.end()){
		logic = itor->second;
	}
	else{
		auto playerItor = _players.find(actorId);
		if (playerItor != _players.end()){
			logic = playerItor->second;
		}
		else{
			logic = _capacity->choose(NodeType::LOGIC);
			//ECHO_ERROR("choose logic %d", logic);
		}

		if (logic > 0)
			_distributes[actorId] = logic;
	}

	sl::BStream<128> res;
	res << agentId << actorId << logic;
	_harbor->send(nodeType, nodeId, NodeProtocol::SCENEMGR_MSG_DISTRIBUTE_LOGIC_ACK, res.out());
}

void Distribution::onLogicAddPlayer(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int64 actorId = args.getInt64(0);
	_players[actorId] = nodeId;
	_distributes.erase(actorId);
}

void Distribution::onLogicRemovePlayer(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int64 actorId = args.getInt64(0);
	_players.erase(actorId);
}
