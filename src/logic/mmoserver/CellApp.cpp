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
    
    logic_event::CreateCellEntity createInfo { entityType, entityId, nodeId};
    SLMODULE(EventEngine)->execEvent(logic_event::EVENT_CREATE_CELL_ENTITY, &createInfo, sizeof(createInfo));

    IObject* entity = SLMODULE(ObjectMgr)->findObject(entityId);
    if(!entity)
        return;

    sl::OBMap cellDataStream(cellData, cellDataSize);
    printf("cell entity %s has attr %d, datasize:%d\n", entityType, entity->getObjProps().size(), cellDataSize);
	for (const IProp* prop : entity->getObjProps()){
		switch (prop->getType(entity)){
		case DTYPE_INT8: entity->setPropInt8(prop, cellDataStream.getInt8(prop->getName())); break;
	    case DTYPE_INT16: entity->setPropInt16(prop, cellDataStream.getInt16(prop->getName())); break;
	    case DTYPE_INT32: entity->setPropInt32(prop, cellDataStream.getInt32(prop->getName())); break;
	    case DTYPE_INT64: entity->setPropInt64(prop, cellDataStream.getInt64(prop->getName())); break;
	    case DTYPE_FLOAT: entity->setPropFloat(prop, cellDataStream.getFloat(prop->getName())); break;
	    case DTYPE_STRING: entity->setPropString(prop, cellDataStream.getString(prop->getName())); break;
	    case DTYPE_BLOB: {
			int32 size = 0;
			const void* p = cellDataStream.getBlob(prop->getName(), size);
			if (p)
				entity->setPropBlob(prop, p, size);

		    } 
		    break;
	    }
    }
    
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

