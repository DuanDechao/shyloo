#include "RangeTrigger.h"
#include "RangeTriggerNode.h"
#include "CoordinateSystem.h"
#include <math.h>
#include <float.h>

RangeTrigger::RangeTrigger(CoordinateNode* origin, float xz, float y)
    :_rangeXZ(fabs(xz)),
     _rangeY(fabs(y)),
     _origin(origin),
     _positiveBoundary(NULL),
     _negativeBoundary(NULL),
     _removing(false)
{
}

RangeTrigger::~RangeTrigger(){
    uninstall();
}

bool RangeTrigger::install(){
    if(_positiveBoundary == NULL)
        _positiveBoundary = NEW RangeTriggerNode(this, 0, 0, true);
    else
        _positiveBoundary->setRange(0.0f, 0.0f);

    if(_negativeBoundary == NULL)
        _negativeBoundary = NEW RangeTriggerNode(this, 0, 0, false);
    else
        _negativeBoundary->setRange(0.0f, 0.0f);

    _positiveBoundary->addFlags(CoordinateNode::COORDINATE_NODE_FLAG_INSTALLING);
    _negativeBoundary->addFlags(CoordinateNode::COORDINATE_NODE_FLAG_INSTALLING);

    _origin->coordinateSystem()->insert(_positiveBoundary);
    _origin->coordinateSystem()->insert(_negativeBoundary);

    _negativeBoundary->setOldX(-FLT_MAX);
    _negativeBoundary->setOldY(-FLT_MAX);
    _negativeBoundary->setOldZ(-FLT_MAX);
    _negativeBoundary->setRange(-_rangeXZ, -_rangeY);
    _negativeBoundary->setOldRange(-_rangeXZ, -_rangeY);
    _negativeBoundary->update();
    
    if(!_negativeBoundary)
        return false;

    _negativeBoundary->removeFlags(CoordinateNode::COORDINATE_NODE_FLAG_INSTALLING);


    _positiveBoundary->setOldX(-FLT_MAX);
    _positiveBoundary->setOldY(-FLT_MAX);
    _positiveBoundary->setOldZ(-FLT_MAX);
    _positiveBoundary->setRange(_rangeXZ, _rangeY);
    _positiveBoundary->setOldRange(_rangeXZ, _rangeY);
    _positiveBoundary->update();

    if(!_positiveBoundary)
        return false;

    _positiveBoundary->removeFlags(CoordinateNode::COORDINATE_NODE_FLAG_INSTALLING);

    return true;
}

bool RangeTrigger::uninstall(){
    if(_removing)
        return false;

    _removing = true;

    if(_positiveBoundary && _positiveBoundary->coordinateSystem()){
        _positiveBoundary->coordinateSystem()->remove(_positiveBoundary);
        _positiveBoundary->onTriggerUninstall();
    }

    if(_negativeBoundary && _negativeBoundary->coordinateSystem()){
        _negativeBoundary->coordinateSystem()->remove(_negativeBoundary);
        _negativeBoundary->onTriggerUninstall();
    }

    _positiveBoundary = NULL;
    _negativeBoundary = NULL;
    _removing = false;
    return true;
}

bool RangeTrigger::reinstall(CoordinateNode* pCoordinateNode){
    uninstall();
    _origin = pCoordinateNode;
    return install();
}

void RangeTrigger::onNodePassX(RangeTriggerNode* pRangeTriggerNode, CoordinateNode* pNode){
    if(pNode == origin())
        return;

    bool wasInZ = pRangeTriggerNode->wasInZRange(pNode);
    bool isInZ = pRangeTriggerNode->isInZRange(pNode);

    if(wasInZ != isInZ)
        return;
    
    bool wasInY = pRangeTriggerNode->wasInYRange(pNode);
    bool isInY = pRangeTriggerNode->isInYRange(pNode);

    if(wasInY != isInY)
        return;

    bool wasIn = pRangeTriggerNode->wasInXRange(pNode) && wasInY && wasInZ;
    bool isIn = pRangeTriggerNode->isInXRange(pNode) && isInY && isInZ;

    if(wasIn == isIn)
        return;

    if(isIn){
        this->onEnter(pNode);
    }
    else{
        this->onLeave(pNode);
    }
}

void RangeTrigger::onNodePassY(RangeTriggerNode* pRangeTriggerNode, CoordinateNode* pNode){
    if(pNode == origin())
        return;

    bool wasInZ = pRangeTriggerNode->wasInZRange(pNode);
    bool isInZ = pRangeTriggerNode->isInZRange(pNode);
    
    if(wasInZ != isInZ)
        return;
    
    bool wasInY = pRangeTriggerNode->wasInYRange(pNode);
    bool isInY = pRangeTriggerNode->isInYRange(pNode);

    if(wasInY == isInY)
        return;

    bool wasIn = pRangeTriggerNode->wasInXRange(pNode) && wasInY && wasInZ;
    bool isIn = pRangeTriggerNode->isInXRange(pNode) && isInY && isInZ;

    if(wasIn == isIn)
        return;

    if(isIn){
        this->onEnter(pNode);
    }
    else{
        this->onLeave(pNode);
    }
}

void RangeTrigger::onNodePassZ(RangeTriggerNode* pRangeTriggerNode, CoordinateNode* pNode){
    if(pNode == origin())
        return;

    bool wasInZ = pRangeTriggerNode->wasInZRange(pNode);
    bool isInZ = pRangeTriggerNode->isInZRange(pNode);
    
    if(wasInZ == isInZ)
        return;

    bool wasIn = pRangeTriggerNode->wasInXRange(pNode) && pRangeTriggerNode->wasInYRange(pNode) && wasInZ;
    bool isIn = pRangeTriggerNode->isInXRange(pNode) && pRangeTriggerNode->isInYRange(pNode) && isInZ;

    if(wasIn == isIn)
        return;

    if(isIn){
        this->onEnter(pNode);
    }else{
        this->onLeave(pNode);
    }
}

void RangeTrigger::update(float xz, float y){
    float oldRangeXZ = _rangeXZ;
    float oldRangeY = _rangeY;

    setRange(xz, y);

    if(_positiveBoundary){
        _positiveBoundary->setRange(_rangeXZ, _rangeY);
        _positiveBoundary->setOldRange(oldRangeXZ, oldRangeY);
        _positiveBoundary->update();
    }

    if(_negativeBoundary){
        _negativeBoundary->setRange(-_rangeXZ, -_rangeY);
        _negativeBoundary->setOldRange(-oldRangeXZ, -oldRangeY);
        _negativeBoundary->update();
    }
}
