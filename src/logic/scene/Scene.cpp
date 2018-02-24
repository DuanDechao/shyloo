#include "Scene.h"
#include "IDCCenter.h"
#include "IEntityDef.h"
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
    test();
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
            SLMODULE(ObjectMgr)->setObjectTypeSize(1);
	        _propX = SLMODULE(ObjectMgr)->appendObjectProp("Avatar", "posX", DTYPE_FLOAT, sizeof(float), 0, 0, 0);
            _propY = SLMODULE(ObjectMgr)->appendObjectProp("Avatar", "posY", DTYPE_FLOAT, sizeof(float), 0, 0, 0);
            _propZ = SLMODULE(ObjectMgr)->appendObjectProp("Avatar", "posZ", DTYPE_FLOAT, sizeof(float), 0, 0, 0); 
            _propCoordinateNode = SLMODULE(ObjectMgr)->appendObjectProp("Avatar", "coordinateNode", DTYPE_INT64, sizeof(int64), 0, 0, 0);
            _propSceneNodeData = SLMODULE(ObjectMgr)->appendObjectProp("Avatar", "sceneNodeData", DTYPE_INT64, sizeof(int64), 0, 0, 0);
            _propPosChangeTimer = SLMODULE(ObjectMgr)->appendObjectTempProp("Avatar", "psoChangeTimer", DTYPE_INT64, sizeof(int64), 0, 0, 0);
   // }
}


void Scene::test(){
    IObject * object = CREATE_OBJECT(SLMODULE(ObjectMgr), "Avatar");
    RGS_PROP_CHANGER(object, _propX, Scene::positionChanged);
    IObject * object1 = CREATE_OBJECT(SLMODULE(ObjectMgr), "Avatar");
    Space* space = NEW Space(1);
    space->addObjectToNode(object);
//    printf("test propNode :%p\n", object->getPropInt64(_propCoordinateNode));

  //  START_OBJECT_TIMER(SLMODULE(ObjectTimer), object, _propPosChangeTimer, 0, -1, 2000, Scene::timerStart, Scene::onTimer, Scene::timerEnd);
    Witness* witness = NEW Witness();
    witness->attach(object);
    space->addObjectToNode(object1);
//    printf("test pos :%p %p %f\n", object,_propX, object->getPropFloat(_propX));

}

void Scene::positionChanged(sl::api::IKernel* pKernel, IObject* object, const char* name, const IProp* prop, const bool sync){
    ObjectCoordinateNode* objectCoorNode = (ObjectCoordinateNode*)object->getPropInt64(_propCoordinateNode);
    if(!objectCoorNode)
        return;

    objectCoorNode->update();
    printf("CoordinateNode updated...\n");
}

void Scene::timerStart(sl::api::IKernel* pKernel, IObject* object, int64 tick){
}

void Scene::onTimer(sl::api::IKernel* pKernel, IObject* object, int64 tick){
    float originX = object->getPropFloat(_propX);
    object->setPropFloat(_propX, originX + 1);
    printf("test position changed\n");
}

void Scene::timerEnd(sl::api::IKernel* pKernel, IObject* object, bool novolient, int64 tick){
}
