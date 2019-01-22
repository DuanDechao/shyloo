#include "DBMgr.h"
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
#include "slargs.h"

bool DBMgr::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;

	return true;
}

bool DBMgr::launched(sl::api::IKernel * pKernel){
    if(SLMODULE(Harbor)->getNodeType() != NodeType::DATABASE)
        return true;

    RGS_NODE_HANDLER(SLMODULE(Harbor), NodeProtocol::BASE_MSG_QUERY_ENTITY, DBMgr::onBaseMsgQueryEntity);

	return true;
}

bool DBMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void DBMgr::onBaseMsgQueryEntity(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const sl::OBStream& args){
    const char* entityType = nullptr;
    uint64 dbid = 0;
    uint64 callbackId = 0;
	args >> entityType >> dbid >> callbackId;

    bool success = false;
    bool wasActive = false;

    //读取数据
    printf("query entity data....................%s, %llu, %llu\n", entityType, dbid);

	sl::BStream<1000> inArgs;
    inArgs << entityType;
    inArgs << dbid;
    inArgs << callbackId;
    inArgs << success;
    inArgs << wasActive;
    SLMODULE(Harbor)->send(nodeType, nodeId, NodeProtocol::DB_MSG_QUERY_ENTITY_CALLBACK, inArgs.out());
}

