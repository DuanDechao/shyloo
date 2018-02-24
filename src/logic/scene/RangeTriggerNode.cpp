#include "RangeTriggerNode.h"
#include "ObjectCoordinateNode.h"

RangeTriggerNode::RangeTriggerNode(RangeTrigger* pRangeTrigger, float xz, float y, bool positiveBoundary)
    :CoordinateNode(NULL),
     _rangeXZ(xz),
     _rangeY(y),
     _oldRangeXZ(_rangeXZ),
     _oldRangeY(_rangeY),
     _rangeTrigger(pRangeTrigger)
{
    if(positiveBoundary){
        setFlags(CoordinateNode::COORDINATE_NODE_FLAG_HIDE | CoordinateNode::COORDINATE_NODE_FLAG_POSITIVE_BOUNDARY);
    }
    else{
        setFlags(CoordinateNode::COORDINATE_NODE_FLAG_HIDE | CoordinateNode::COORDINATE_NODE_FLAG_NEGATIVE_BOUNDARY);
    }
    
    static_cast<ObjectCoordinateNode*>(_rangeTrigger->origin())->addWatcherNode(this);
}

RangeTriggerNode::~RangeTriggerNode(){}


void RangeTriggerNode::onTriggerUninstall(){
    if(_rangeTrigger->origin()){
        static_cast<ObjectCoordinateNode*>(_rangeTrigger->origin())->delWatcherNode(this);
    }

    setRangeTrigger(NULL);
}

void RangeTriggerNode::onRemove(){
    CoordinateNode::onRemove();

    if(_rangeTrigger)
        _rangeTrigger->uninstall();
}

float RangeTriggerNode::xx() const{
    if(hasFlags(CoordinateNode::COORDINATE_NODE_FLAG_REMOVED | CoordinateNode::COORDINATE_NODE_FLAG_REMOVING)
        || _rangeTrigger == NULL)
        return -FLT_MAX;

    return _rangeTrigger->origin()->xx() + _rangeXZ;
}

float RangeTriggerNode::yy() const{
    if(hasFlags(CoordinateNode::COORDINATE_NODE_FLAG_REMOVED | CoordinateNode::COORDINATE_NODE_FLAG_REMOVING)
        || _rangeTrigger == NULL)
        return -FLT_MAX;

    return _rangeTrigger->origin()->yy() + _rangeY;
}

float RangeTriggerNode::zz() const{
    if(hasFlags(CoordinateNode::COORDINATE_NODE_FLAG_REMOVED | CoordinateNode::COORDINATE_NODE_FLAG_REMOVING)
        || _rangeTrigger == NULL)
        return -FLT_MAX;

    return _rangeTrigger->origin()->zz() + _rangeXZ;
}

void RangeTriggerNode::onNodePassX(CoordinateNode* pNode){
    if(!hasFlags(CoordinateNode::COORDINATE_NODE_FLAG_REMOVED) && _rangeTrigger)
        _rangeTrigger->onNodePassX(this, pNode);
}

void RangeTriggerNode::onNodePassY(CoordinateNode* pNode){
    if(!hasFlags(CoordinateNode::COORDINATE_NODE_FLAG_REMOVED) && _rangeTrigger)
        _rangeTrigger->onNodePassY(this, pNode);
}

void RangeTriggerNode::onNodePassZ(CoordinateNode* pNode){
    if(!hasFlags(CoordinateNode::COORDINATE_NODE_FLAG_REMOVED) && _rangeTrigger)
        _rangeTrigger->onNodePassZ(this, pNode);
}

