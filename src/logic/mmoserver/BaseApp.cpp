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
#include "slbinary_stream.h"
#include "IObjectDef.h"
#include "IEntityMgr.h"
#include "IGlobalData.h"

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
	RGS_GATE_ARGS_HANDLER(SLMODULE(Gate), ClientMsgID::CLIENT_MSG_REMOTE_METHOD_CALL, BaseApp::onClientRemoteMethodCall);

	return true;
}

bool BaseApp::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}


void BaseApp::onGateLogined(sl::api::IKernel* pKernel, const int64 id){
    const char* proxyScriptType = "Account";
	IObject* proxy = CREATE_OBJECT(SLMODULE(ObjectMgr), proxyScriptType);

    logic_event::ProxyCreated info { proxy, id};
    SLMODULE(EventEngine)->execEvent(logic_event::EVENT_PROXY_CREATED, &info, sizeof(info));
    exportObjectDef(id);
    proxyCreated(proxy, id);
}

void BaseApp::exportObjectDef(const int64 id){
    sl::BStream<40960> args;
    SLMODULE(ObjectDef)->addAllObjectDefToStream(args);
    SLMODULE(Gate)->sendToClient(id, ServerMsgID::SERVER_MSG_OBJECT_DEF_EXPORT, args.out());
}

void BaseApp::proxyCreated(IObject* object, const int64 id){
    sl::BStream<512> args;
    args << (int64)object->getID();
    args << (int32)343422;
    args << object->getObjTypeString();
    SLMODULE(Gate)->sendToClient(id, ServerMsgID::SERVER_MSG_PROXY_CREATED, args.out());
	_idToObjects[id] = object;
}

void BaseApp::onGateLogOnAttempt(sl::api::IKernel* pKernel, const int64 id){
}

int32 BaseApp::onGateMsgRecv(sl::api::IKernel* pKernel, const int64 id, const void* context, const int32 size){
    return 0;
}

void BaseApp::onGateUnbind(sl::api::IKernel* pKernel, const int64 id){
}

bool BaseApp::remoteCreateCellEntity(IObject* object, IObject* createFromObject, const int32 cellIdx, const void* cellData, const int32 cellDataSize){
    if(SLMODULE(Harbor)->getNodeType() != NodeType::LOGIC)
        return false;

	bool hasClient = true;
    const char* entityType = object->getObjTypeString();
    IArgs<20, 2048> args;
	args << (createFromObject ? false : true); 
    args << entityType;
    args << object->getID();
	args << (createFromObject ? createFromObject->getID() : (int64)1); 
	args << hasClient; 
    args.addStruct(cellData, cellDataSize);
    args.fix();
    SLMODULE(Harbor)->send(NodeType::SCENE, cellIdx, NodeProtocol::BASE_MSG_CREATE_CELL_ENTITY, args.out());
    
    return true;
}

void BaseApp::onCellEntityCreatedFromCell(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
    const int64 entityId = args.getInt64(0);
    logic_event::CellEntityCreatedFromCell info { entityId, nodeId};
    SLMODULE(EventEngine)->execEvent(logic_event::EVENT_CELL_ENTITY_CREATED, &info, sizeof(info));
}

bool BaseApp::createEntityFromDB(const char* entityType, const uint64 dbid, const uint64 callbackId, const uint64 entityId){
    IArgs<20, 1000> args;
    args << entityType;
    args << dbid;
    args << callbackId;
    args << entityId;
    args.fix();
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

void BaseApp::onClientRemoteMethodCall(sl::api::IKernel* pKernel, const int64 id, const sl::OBStream& args){
	auto itor = _idToObjects.find(id);
	if(itor == _idToObjects.end()){
		ECHO_ERROR("cant find agentid[%lld]", id);
		return;
	}

	const uint64 objectId = itor->second->getID();
	SLMODULE(EntityMgr)->onRemoteMethodCall(objectId, args);
}

