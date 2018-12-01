#include "BaseAppMgr.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "ICluster.h"
bool BaseAppMgr::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	_forwardMsgBuffer = NULL;

	return true;
}

bool BaseAppMgr::launched(sl::api::IKernel * pKernel){
    if(SLMODULE(Harbor)->getNodeType() != NodeType::MASTER)
        return true;

	if(!_forwardMsgBuffer)
		_forwardMsgBuffer = NEW ForwardMsgBuffer(pKernel,NodeType::LOGIC);

    RGS_NODE_HANDLER(SLMODULE(Harbor), NodeProtocol::BASE_MSG_CREATE_BASE_ANYWHERE, BaseAppMgr::onBaseMsgReqCreateBaseAnywhere);

	return true;
}

bool BaseAppMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void BaseAppMgr::onBaseMsgReqCreateBaseAnywhere(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const sl::OBStream& args){
	int32 logic = SLMODULE(Cluster)->getBestNodeId(NodeType::LOGIC);
	printf("ddddddddddddddddddddddddddddddd %d\n", logic);
	if(logic <= 0 || (logic != nodeId && SLMODULE(Cluster)->getNodeState(NodeType::LOGIC, logic) != ServerState::S_RUNNING)){
		ForwardMsg* msg = NEW ForwardMsg(nodeId, NodeProtocol::BASEMGR_MSG_CREATE_BASE_ANYWHERE, args.getContext(), args.getSize());
		_forwardMsgBuffer->push(msg);
	}
	else{
		_forwardMsgBuffer->process();
		SLMODULE(Harbor)->send(NodeType::LOGIC, logic, NodeProtocol::BASEMGR_MSG_CREATE_BASE_ANYWHERE, args);
	}
}
