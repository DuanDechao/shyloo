#include "CapacitySubscriber.h"
#include "slxml_reader.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"

bool CapacitySubscriber::initialize(sl::api::IKernel * pKernel){
	return true;
}

bool CapacitySubscriber::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);

	RGS_NODE_HANDLER(_harbor, NodeProtocol::NODE_CAPACITY_LOAD_REPORT, CapacitySubscriber::nodeLoadReport);
	
	return true;
}

bool CapacitySubscriber::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

int32 CapacitySubscriber::choose(int32 nodeType){
	return 1;
}

void CapacitySubscriber::nodeLoadReport(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	float nodeLoad = args.getFloat(0);
	_allNodeLoad[nodeType][nodeId] = { nodeLoad };
	ECHO_ERROR("node[%d:%d] load updated: %f", nodeType, nodeId, nodeLoad);
}
