#include "CellAppMgr.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
bool CellAppMgr::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;

	return true;
}

bool CellAppMgr::launched(sl::api::IKernel * pKernel){
    if(SLMODULE(Harbor)->getNodeType() != NodeType::MASTER)
        return true;

    RGS_NODE_ARGS_HANDLER(SLMODULE(Harbor), NodeProtocol::BASE_MSG_CREATE_IN_NEW_SPACE, CellAppMgr::onReqCreateInNewSpaceFromBase);

	return true;
}

bool CellAppMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}
	
void CellAppMgr::onReqCreateInNewSpaceFromBase(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
	const char* entityType = args.getString(0);
	const uint64 entityId = args.getInt64(1);
	const int32 cellIdx = args.getInt32(2);
	const bool hasClient = args.getBool(3);
	int32 size = 0;
	const void* cellData = args.getStruct(4, size);

	static int32 spaceID = 1;
	IArgs<20, 2048> inArgs;
	inArgs << true; 
    inArgs << entityType;
    inArgs << entityId;
	inArgs << (uint64)(spaceID++); 
	inArgs << hasClient;
	inArgs.addStruct(cellData, size);
	inArgs << nodeId;
    inArgs.fix();
    SLMODULE(Harbor)->send(NodeType::SCENE, cellIdx, NodeProtocol::BASE_MSG_CREATE_CELL_ENTITY, inArgs.out());
}
