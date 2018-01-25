#include "BaseApp.h"
#include "ProtocolID.pb.h"
#include "IDCCenter.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "IEventEngine.h"
#include "EventID.h"
#include "slstring.h"
#include "GameDefine.h"
#include "IIdMgr.h"

bool BaseApp::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;

	return true;
}

bool BaseApp::launched(sl::api::IKernel * pKernel){
    if(SLMODULE(Harbor)->getNodeType() != NodeType::LOGIC)
        return true;

    SLMODULE(Gate)->rgsGateListener(this);    
    
    RGS_NODE_ARGS_HANDLER(SLMODULE(Harbor), NodeProtocol::CELL_MSG_CELL_ENTITY_CREATED, BaseApp::onCellEntityCreatedFromCell);
    RGS_NODE_ARGS_HANDLER(SLMODULE(Harbor), NodeProtocol::DB_MSG_QUERY_ENTITY_CALLBACK, BaseApp::onDBMsgQueryEntityCallback);

	return true;
}

bool BaseApp::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}


void BaseApp::onGateLogined(sl::api::IKernel* pKernel, const int64 id){
    uint64 proxyId = SLMODULE(IdMgr)->allocID();
    const char* proxyScriptType = "LoginProxy";
    logic_event::GateLoginedInfo loginedInfo { proxyScriptType, proxyId, id};
    SLMODULE(EventEngine)->execEvent(logic_event::EVENT_GATE_LOGINED, &loginedInfo, sizeof(loginedInfo));
}

void BaseApp::onGateLogOnAttempt(sl::api::IKernel* pKernel, const int64 id){
}

int32 BaseApp::onGateMsgRecv(sl::api::IKernel* pKernel, const int64 id, const void* context, const int32 size){
    return 0;
}

void BaseApp::onGateUnbind(sl::api::IKernel* pKernel, const int64 id){
}

bool BaseApp::remoteCreateCellEntity(IObject* object, const int32 cellIdx, const void* cellData, const int32 cellDataSize){
    if(SLMODULE(Harbor)->getNodeType() != NodeType::LOGIC)
        return false;

    const char* entityType = object->getObjTypeString();
    IArgs<20, 2048> args;
    args << entityType;
    args << object->getID();
    args.addStruct(cellData, cellDataSize);
    args.fix();
    SLMODULE(Harbor)->send(NodeType::SCENE, cellIdx, NodeProtocol::BASE_MSG_CREATE_CELL_ENTITY, args.out());
    
    return true;
}

void BaseApp::onCellEntityCreatedFromCell(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
    const int64 entityId = args.getInt64(0);
    logic_event::CellEntityCreated info { entityId, nodeId};
    SLMODULE(EventEngine)->execEvent(logic_event::EVENT_CELL_ENTITY_CREATED, &info, sizeof(info));
}

bool BaseApp::createEntityFromDB(const char* entityType, const uint64 dbid, const uint64 callbackId, const uint64 entityId){
    IArgs<20, 1000> args;
    args << entityType;
    args << dbid;
    args << callbackId;
    args << entityId;
    args.fix();
    printf("createEntityFromDB starting.......\n");
    SLMODULE(Harbor)->send(NodeType::DATABASE, 1, NodeProtocol::BASE_MSG_QUERY_ENTITY, args.out());
    return true;
}

void BaseApp::onDBMsgQueryEntityCallback(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
    const char* entityType = args.getString(0);
    const uint64 dbid = args.getInt64(1);
    const uint64 callbackId = args.getInt64(2);
    const uint64 entityId = args.getInt64(3);
    bool success = args.getBool(4);
    bool wasActive = args.getBool(5);

    logic_event::EntityCreatedFromDBCallBack info {entityType, dbid, callbackId, entityId, success, wasActive};
    SLMODULE(EventEngine)->execEvent(logic_event::EVENT_ENTITY_CREATED_FROM_DB_CALLBACK, &info, sizeof(info));
}
