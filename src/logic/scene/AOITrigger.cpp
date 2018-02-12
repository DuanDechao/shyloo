#include "AOITrigger.h"

AOITrigger::AOITrigger(CoordinateNode* origin, float xz, float y)
    :RangeTrigger(origin, xz, y),
     _witness(NULL)
{}

AOITrigger::~AOITrigger()
{}


void AOITrigger::onEnter(CoordinateNode* pNode){
    if(pNode->flags() & CoordinateNode::COORDINATE_NODE_FLAG_OBJECT <= 0)
        return;

    ObjectCoordinateNode* pObjectCoordinateNode = static_cast<ObjectCoordinateNode*>(pNode);

}

void AOITrigger::onLeave(CoordinateNode* pNode){
}
