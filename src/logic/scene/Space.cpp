#include "Space.h"
#include "ObjectCoordinateNode.h"
#include "IDCCenter.h"
#include "Scene.h"
Space::Space(const int32 spaceId)
    :_id(spaceId)
{}

Space::~Space(){}


void Space::addObject(IObject* object){
    _spaceObjects.push_back(object);    
}

void Space::addObjectToNode(IObject* object){
    const IProp* nodeProp = Scene::getInstance()->getPropCoordinateNode();
    ObjectCoordinateNode* objectCoorNode = (ObjectCoordinateNode*)object->getPropInt64(nodeProp);
    if(!objectCoorNode){
        objectCoorNode = NEW ObjectCoordinateNode(object);
        object->setPropInt64(nodeProp, (int64)objectCoorNode);
    }

    _coordinateSystem.insert(objectCoorNode); 
}
