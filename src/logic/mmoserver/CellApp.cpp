#include "CellApp.h"
#include "IDCCenter.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "IEventEngine.h"
#include "EventID.h"
#include "slstring.h"
#include "GameDefine.h"
#include "slbinary_map.h"
#include "IEntityMgr.h"

bool CellApp::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	return true;
}

bool CellApp::launched(sl::api::IKernel * pKernel){
    if(SLMODULE(Harbor)->getNodeType() != NodeType::SCENE)
        return true;

    RGS_NODE_ARGS_HANDLER(SLMODULE(Harbor), NodeProtocol::BASE_MSG_CREATE_CELL_ENTITY, CellApp::onCreateCellEntityFromBase);

	return true;
}

bool CellApp::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void CellApp::onCreateCellEntityFromBase(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
    const char* entityType = args.getString(0);
    const int64 entityId = args.getInt64(1);
    int32 cellDataSize = 0;
    const void* cellData = args.getStruct(2, cellDataSize);

    //logic_event::CreateCellEntity createInfo { entityType, entityId, nodeId};
    //SLMODULE(EventEngine)->execEvent(logic_event::EVENT_CREATE_CELL_ENTITY, &createInfo, sizeof(createInfo));
    SLMODULE(EntityMgr)->createCellEntity(entityType, cellData, cellDataSize, nodeId, entityId);
    
    IArgs<2, 256> inArgs;
    inArgs << entityId;
    inArgs.fix();
    SLMODULE(Harbor)->send(nodeType, nodeId, NodeProtocol::CELL_MSG_CELL_ENTITY_CREATED, inArgs.out());
}


bool CellApp::createEntity(const char* entityName, const uint64 entityId){
	IObject* object = entityId != 0 ? CREATE_OBJECT_BYID(SLMODULE(ObjectMgr), entityName, entityId) : CREATE_OBJECT(SLMODULE(ObjectMgr), entityName);
    
    logic_event::Biology info { object };
    SLMODULE(EventEngine)->execEvent(logic_event::EVENT_NEW_OBJECT_CREATED, &info, sizeof(info));

    return true;
}

