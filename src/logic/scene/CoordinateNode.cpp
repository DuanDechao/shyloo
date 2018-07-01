#include "CoordinateNode.h"
#include "CoordinateSystem.h"
#include <float.h>
CoordinateNode::CoordinateNode(CoordinateSystem* pCoordinateSystem)
    :_prevX(NULL),
     _prevY(NULL),
     _prevZ(NULL),
     _nextX(NULL),
     _nextY(NULL),
     _nextZ(NULL),
     _coordinateSystem(pCoordinateSystem),
     _x(-FLT_MAX),
     _y(-FLT_MAX),
     _z(-FLT_MAX),
     _oldx(-FLT_MAX),
     _oldy(-FLT_MAX),
     _oldz(-FLT_MAX),
     _flags(COORDINATE_NODE_FLAG_UNKNOWN)
{}

CoordinateNode::~CoordinateNode(){
    //if()
}

void CoordinateNode::setCoordinateSystem(CoordinateSystem* sys){
    _coordinateSystem = sys;
}

void CoordinateNode::onRemove(){
    setOldX(_x);
    setOldY(_y);
    setOldZ(_z);

    _x = -FLT_MAX;
}

void CoordinateNode::update(){
    if(_coordinateSystem)
        _coordinateSystem->update(this);
}
