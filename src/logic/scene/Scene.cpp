#include "Scene.h"
bool Scene::initialize(sl::api::IKernel * pKernel){
	_self = this;
	return true;
}

bool Scene::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);

	if (_harbor->getNodeType() != NodeType::SCENE)
		return true;

	FIND_MODULE(_objectMgr, ObjectMgr);
	FIND_MODULE(_capacitySubscriber, CapacitySubscriber);
	FIND_MODULE(_capacityPublisher, CapacityPublisher);
	FIND_MODULE(_eventEngine, EventEngine);

	_scenes = CREATE_STATIC_TABLE(_objectMgr, OCStaticTableMacro::SCENECOPY::TABLE_NAME, OCStaticTableMacro::SCENECOPY::TABLE_NAME);

	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::SCENEMGR_MSG_CREATE_SCENE, Scene::onSceneMgrCreateScene);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::SCENEMGR_MSG_ENTER_SCENE, Scene::onSceneMgrEnterScene);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::SCENEMGR_MSG_APPEAR_SCENE, Scene::onSceneMgrAppearScene);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::SCENEMGR_MSG_LEAVE_SCENE, Scene::onSceneMgrLeaveScene);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::SCENEMGR_MSG_SYNC_SCENE, Scene::onSceneMgrSyncScene);

	RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_SCENE_ENTER_SCENE, Scene::onPlayerEnterScene);

	return true;
}

bool Scene::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

bool Scene::appendSceneProp(){
    std::vector<IScriptDefModule*>& modules = SLMODULE(EntityDef)->getAllScriptDefModule();    
    for(int32 i = 0; i < modules.size(); i++){
        IScriptDefModule* pScriptModule = modules[i];
        if(pScriptModule->hasClient())
	        _propX = SLMODULE(ObjectMgr)->appendObjectProp(pScriptModule->getModuleName(), "posX", DTYPE_FLOAT, sizeof(float), 0, 0, 0);
            _propY = SLMODULE(ObjectMgr)->appendObjectProp(pScriptModule->getModuleName(), "posY", DTYPE_FLOAT, sizeof(float), 0, 0, 0);
            _propZ = SLMODULE(ObjectMgr)->appendObjectProp(pScriptModule->getModuleName(), "posZ", DTYPE_FLOAT, sizeof(float), 0, 0, 0); 
    }
}

