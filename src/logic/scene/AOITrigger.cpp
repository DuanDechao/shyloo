#include "AOITrigger.h"
#include "ObjectCoordinateNode.h"
#include "Witness.h"
AOITrigger::AOITrigger(CoordinateNode* origin, float xz, float y)
    :RangeTrigger(origin, xz, y),
     _witness(NULL)
{
     _witness = (Witness*)(static_cast<ObjectCoordinateNode*>(origin)->object()->getPropInt64(Scene::getInstance()->getPropWitness()));
}

AOITrigger::~AOITrigger()
{}


void AOITrigger::onEnter(CoordinateNode* pNode){
    if(pNode->flags() & CoordinateNode::COORDINATE_NODE_FLAG_OBJECT <= 0)
        return;

    ObjectCoordinateNode* pObjectCoordinateNode = static_cast<ObjectCoordinateNode*>(pNode);
    //printf("AOITrigger try to enter\n");

    _witness->onEnterAOI(this, pObjectCoordinateNode->object());

}

void AOITrigger::onLeave(CoordinateNode* pNode){
    if(pNode->flags() & CoordinateNode::COORDINATE_NODE_FLAG_OBJECT <= 0)
        return;

    ObjectCoordinateNode* pObjectCoordinateNode = static_cast<ObjectCoordinateNode*>(pNode);
    //printf("AOITrigger try to leave\n");

    _witness->onLeaveAOI(this, pObjectCoordinateNode->object());

}
