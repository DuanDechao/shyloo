#include "BaseAppMgr.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "ICapacity.h"
bool BaseAppMgr::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;

	return true;
}

bool BaseAppMgr::launched(sl::api::IKernel * pKernel){
    if(SLMODULE(Harbor)->getNodeType() != NodeType::MASTER)
        return true;

    RGS_NODE_ARGS_HANDLER(SLMODULE(Harbor), NodeProtocol::BASE_MSG_CREATE_BASE_ANYWHERE, BaseAppMgr::onBaseMsgReqCreateBaseAnywhere);

	return true;
}

bool BaseAppMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void BaseAppMgr::onBaseMsgReqCreateBaseAnywhere(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
	int32 logic = SLMODULE(CapacitySubscriber)->choose(NodeType::LOGIC);
	SLMODULE(Harbor)->send(NodeType::LOGIC, logic, NodeProtocol::BASEMGR_MSG_CREATE_BASE_ANYWHERE, args);
}
