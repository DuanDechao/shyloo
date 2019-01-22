#include "Scene.h"
#include "IDCCenter.h"
#include "IObjectDef.h"
#include "ObjectCoordinateNode.h"
#include "Witness.h"
#include "Space.h"
#include "IObjectTimer.h"

Scene* Scene::s_self = nullptr;
bool Scene::initialize(sl::api::IKernel * pKernel){
	s_self = this;
	return true;
}

bool Scene::launched(sl::api::IKernel * pKernel){
    appendSceneProp();
	return true;
}

bool Scene::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

bool Scene::appendSceneProp(){
//    std::vector<IScriptDefModule*>& modules = SLMODULE(EntityDef)->getAllScriptDefModule();    
  //  for(int32 i = 0; i < modules.size(); i++){
     //   IScriptDefModule* pScriptModule = modules[i];
       // if(pScriptModule->hasClient())
    const std::vector<const IObjectDefModule*>& allObjectDefs = SLMODULE(ObjectDef)->getAllObjectDefModule();     
	for(auto defModule : allObjectDefs){
		if(!defModule->hasCell())
			continue;

		const char* moduleName = defModule->getModuleName();
		_propX = SLMODULE(ObjectMgr)->appendObjectProp(moduleName, "posX", DTYPE_FLOAT, sizeof(float), 0, 0, 0);
		_propY = SLMODULE(ObjectMgr)->appendObjectProp(moduleName, "posY", DTYPE_FLOAT, sizeof(float), 0, 0, 0);
		_propZ = SLMODULE(ObjectMgr)->appendObjectProp(moduleName, "posZ", DTYPE_FLOAT, sizeof(float), 0, 0, 0); 
		_propRoll = SLMODULE(ObjectMgr)->appendObjectProp(moduleName, "dirRoll", DTYPE_FLOAT, sizeof(float), 0, 0, 0);
		_propPitch = SLMODULE(ObjectMgr)->appendObjectProp(moduleName, "dirPitch", DTYPE_FLOAT, sizeof(float), 0, 0, 0);
		_propYaw = SLMODULE(ObjectMgr)->appendObjectProp(moduleName, "dirYaw", DTYPE_FLOAT, sizeof(float), 0, 0, 0); 
		_propCoordinateNode = SLMODULE(ObjectMgr)->appendObjectProp(moduleName, "coordinateNode", DTYPE_INT64, sizeof(int64), 0, 0, 0);
		_propSceneNodeData = SLMODULE(ObjectMgr)->appendObjectProp(moduleName, "sceneNodeData", DTYPE_INT64, sizeof(int64), 0, 0, 0);
		_propWitness = SLMODULE(ObjectMgr)->appendObjectProp(moduleName, "witness", DTYPE_INT64, sizeof(int64), 0, 0, 0);
		_propSpaceId = SLMODULE(ObjectMgr)->appendObjectProp(moduleName, "spaceId", DTYPE_INT32, sizeof(int32), 0, 0, 0);

	}

	Space::setPropCoordinateNode(_propCoordinateNode);
	Space::setPropWitness(_propWitness);
	Space::setPropPosZ(_propZ);
	Space::setPropSpaceId(_propSpaceId);
   // }
}

ISpace* Scene::createNewSpace(const uint32 spaceId){
    auto itor = _spaces.find(spaceId);
    if(itor != _spaces.end()){
        return NULL;
    }

    Space* space = NEW Space(spaceId);
    _spaces[spaceId] = space;

    return space;
}

ISpace* Scene::findSpace(const uint32 spaceId){
    auto iter = _spaces.find(spaceId);
    if (iter != _spaces.end())
        return iter->second;

    return NULL;
}

IWitness* Scene::getWitness(IObject* object){
    return (IWitness*)object->getPropInt64(_propWitness);
}

void Scene::updatePosition(IObject* object, float x, float y, float z){
    object->setPropFloat(_propX, x);
    object->setPropFloat(_propY, y);
    object->setPropFloat(_propZ, z);
}

void Scene::getPosition(IObject* object, float& x, float& y, float& z){
    x = object->getPropFloat(_propX);
    y = object->getPropFloat(_propY);
    z = object->getPropFloat(_propZ);
}

void Scene::updateDirection(IObject* object, float roll, float pitch, float yaw){
    object->setPropFloat(_propRoll, roll);
    object->setPropFloat(_propPitch, pitch);
    object->setPropFloat(_propYaw, yaw);
}

void Scene::getDirection(IObject* object, float& roll, float& pitch, float& yaw){
    roll = object->getPropFloat(_propRoll);
    pitch = object->getPropFloat(_propPitch);
    yaw = object->getPropFloat(_propYaw);
}

int32 Scene::getSpaceId(IObject* object){
    return object->getPropInt32(_propSpaceId);
}
