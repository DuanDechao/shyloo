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
#include "IScene.h"
#include "IObjectTimer.h"
bool CellApp::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
    
	if(SLMODULE(Harbor)->getNodeType() != NodeType::SCENE)
        return true;
	
	SLMODULE(GlobalDataClient)->addGlobalDataListener(this);
	return true;
}

bool CellApp::launched(sl::api::IKernel * pKernel){
    if(SLMODULE(Harbor)->getNodeType() != NodeType::SCENE)
        return true;
	
    RGS_NODE_ARGS_HANDLER(SLMODULE(Harbor), NodeProtocol::BASE_MSG_CREATE_CELL_ENTITY, CellApp::onCreateCellEntityFromBase);
    
    _propPosChangeTimer = SLMODULE(ObjectMgr)->appendObjectTempProp("Avatar", "psoChangeTimer", DTYPE_INT64, sizeof(int64), 0, 0, 0);
//    test();
	return true;
}

bool CellApp::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void CellApp::onCreateCellEntityFromBase(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
    bool isInNewSpace = args.getBool(0);
    const char* entityType = args.getString(1);
    const int64 entityId = args.getInt64(2);
	const int64 otherOrSpaceId = args.getInt64(3);
    bool hasClient = args.getBool(4);
    int32 cellDataSize = 0;
    const void* cellData = args.getStruct(5, cellDataSize);

	ISpace* pSpace = nullptr;
	if(!isInNewSpace){
		IObject* pCreateToObject = SLMODULE(ObjectMgr)->findObject(otherOrSpaceId);
		int32 spaceID = SLMODULE(Scene)->getSpaceId(pCreateToObject);
		pSpace = SLMODULE(Scene)->findSpace(spaceID); 
	}
	else{
		pSpace = SLMODULE(Scene)->createNewSpace(otherOrSpaceId); 
	}

    if(!createCellEntityFromBase(pSpace, entityType, entityId, hasClient, nodeId, cellData, cellDataSize)){
        ERROR_LOG("onCreateCellEntityFromBase create cellEntity failed");
        return;
    }
    
    IArgs<2, 256> inArgs;
    inArgs << entityId;
    inArgs.fix();
    SLMODULE(Harbor)->send(nodeType, nodeId, NodeProtocol::CELL_MSG_CELL_ENTITY_CREATED, inArgs.out());
}

bool CellApp::createCellEntityFromBase(ISpace* space, const char* entityType, const int64 entityId, bool hasClient, int32 baseNodeId, const void* cellData, const int32 cellDataSize){
	IObject* object = CREATE_OBJECT_BYID(SLMODULE(ObjectMgr), entityType, entityId);
    if(!object){
        ECHO_ERROR("onCreateCellEntityFromBase create cell Entity failed");
        return false;
    }

    space->addObject(object);
    space->addObjectToNode(object);

    if(hasClient){
        space->setWitness(object);
        onGetWitness(object, space);
    }
    
	logic_event::CellEntityCreated info {object, cellData, cellDataSize, baseNodeId};
    SLMODULE(EventEngine)->execEvent(logic_event::EVENT_CELL_ENTITY_CREATED, &info, sizeof(info));
    
	return true;
}

void CellApp::onGetWitness(IObject* object, ISpace* space){
    if(SLMODULE(Scene)->getWitness(object)){
        addSpaceDataToClient(object, space);
        onEnterSpace(object);
        space->onEnterWorld(object);
    }
}

void CellApp::addSpaceDataToClient(IObject* object, ISpace* space){
    sl::BStream<5000> spaceData;
    space->addSpaceDataToStream(spaceData);

    //TODO发送到客户端
}

void CellApp::onEnterSpace(IObject* object){
    //TODO同步玩家的坐标到客户端
}


bool CellApp::createEntity(const char* entityName, const uint64 entityId){
	IObject* object = entityId != 0 ? CREATE_OBJECT_BYID(SLMODULE(ObjectMgr), entityName, entityId) : CREATE_OBJECT(SLMODULE(ObjectMgr), entityName);
    
    logic_event::Biology info { object };
    SLMODULE(EventEngine)->execEvent(logic_event::EVENT_NEW_OBJECT_CREATED, &info, sizeof(info));

    return true;
}

bool CellApp::addSpaceGeometryMapping(const int32 spaceId, const char* path, bool shouldLoadOnServer, std::map<int32, std::string>& params){
	ISpace* space = SLMODULE(Scene)->findSpace(spaceId);
	if(!space){
		ECHO_ERROR("CellApp::addSpaceGeometryMapping: space[%d] is not exist!", spaceId);
		return false;
	}
	return space->addSpaceGeometryMapping(path, shouldLoadOnServer, params);
}

void CellApp::timerStart(sl::api::IKernel* pKernel, IObject* object, int64 tick){
}

void CellApp::onTimer(sl::api::IKernel* pKernel, IObject* object, int64 tick){
    float x = 0, y = 0, z = 0;
    SLMODULE(Scene)->getPosition(object, x, y, z);
    static int32 step = 1;
    if(z > 3)
        step = -1;
    
    SLMODULE(Scene)->updatePosition(object, x, y, z + step);
    printf("test object[%lld] position changed\n", object->getID());
}

void CellApp::timerEnd(sl::api::IKernel* pKernel, IObject* object, bool novolient, int64 tick){
}

void CellApp::onGlobalDataChanged(const char* key, const int16 dataType, const void* data, const int32 dataSize, bool isDelete){
	printf("CellApp get GlobalData changed: %s : %s\n", key, (const char*)data);
}

void CellApp::test(){
    IArgs<5, 1024> args;
    args << "Avatar";
    args << (const int64)676878434242;
    args << 1;
    args << true;
    args.addStruct(nullptr, 0);
    //onCreateInNewSpaceFromBase(_kernel, 5, 1, args.out());
    
    IArgs<5, 1024> args1;
    args1 << (const int64)676878434242;
    args1 << "Avatar";
    args1 << (const int64)6768784342424;
    args1 << true;
    args1.addStruct(nullptr, 0);
    onCreateCellEntityFromBase(_kernel, 5, 1, args1.out());

    IObject* objectA = SLMODULE(ObjectMgr)->findObject(676878434242);
    START_OBJECT_TIMER(SLMODULE(ObjectTimer), objectA, _propPosChangeTimer, 0, -1, 2000, CellApp::timerStart, CellApp::onTimer, CellApp::timerEnd);
}
