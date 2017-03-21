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

		RGS_NODE_HANDLER(_harbor, NodeProtocol::GATE_MSG_DISTRIBUTE_LOGIC_REQ, Distribution::onGateDistributeLogic);
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
		}

		if (logic > 0)
			_distributes[actorId] = logic;
	}

	IArgs<3, 128> res;
	res << agentId << actorId << logic;
	res.fix();
	_harbor->send(nodeType, nodeId, NodeProtocol::SCENEMGR_MSG_DISTRIBUTE_LOGIC_ACK, res.out());
}

