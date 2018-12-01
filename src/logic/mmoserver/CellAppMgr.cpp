#include "CellAppMgr.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "ICluster.h"
bool CellAppMgr::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	_forwardMsgBuffer = NULL;

	return true;
}

bool CellAppMgr::launched(sl::api::IKernel * pKernel){
    if(SLMODULE(Harbor)->getNodeType() != NodeType::MASTER)
        return true;
	
	if(!_forwardMsgBuffer)
		_forwardMsgBuffer = NEW ForwardMsgBuffer(pKernel,NodeType::SCENE);

    RGS_NODE_HANDLER(SLMODULE(Harbor), NodeProtocol::BASE_MSG_CREATE_IN_NEW_SPACE, CellAppMgr::onReqCreateInNewSpaceFromBase);

	return true;
}

bool CellAppMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}
	
void CellAppMgr::onReqCreateInNewSpaceFromBase(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const sl::OBStream& args){
	// 如果cellIdx为0，则代表不强制指定cellapp
	const char* entityType = nullptr;
	uint64 entityId = 0;
	int32 cellIdx = 0;
	bool hasClient = false;
	args >> entityType >> entityId >> cellIdx >> hasClient;
	int32 size = 0;
	const void* cellData = args.readBlob(size);

	static int32 spaceID = 1;
	sl::BStream<2048> inArgs;
	inArgs << true; 
    inArgs << entityType;
    inArgs << entityId;
	inArgs << (uint64)(spaceID++); 
	inArgs << hasClient;
	inArgs.addBlob(cellData, size);
	inArgs << nodeId;
	sl::OBStream outStream = inArgs.out();

	int32 cellNodeId = cellIdx;
	if(cellNodeId == 0){
		cellNodeId = SLMODULE(Cluster)->getBestNodeId(NodeType::SCENE);
	}
	if(cellNodeId == 0 || (cellNodeId > 0  && SLMODULE(Cluster)->getNodeState(NodeType::SCENE, cellNodeId) != ServerState::S_RUNNING)){
		ForwardMsg* msg = NEW ForwardMsg(cellNodeId, NodeProtocol::BASE_MSG_CREATE_CELL_ENTITY, outStream.getContext(), outStream.getSize());
		_forwardMsgBuffer->push(msg);
	}
	else{
		_forwardMsgBuffer->process();
		SLMODULE(Harbor)->send(NodeType::SCENE, cellNodeId, NodeProtocol::BASE_MSG_CREATE_CELL_ENTITY, outStream);
	}
}
