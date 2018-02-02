#ifndef __SL_FRAMEWORK_OBJECTCOORDINATENODE_H__
#define __SL_FRAMEWORK_OBJECTCOORDINATENODE_H__
#include "CoordinateNode.h"

class ObjectCoordinateNode: public CoordinateNode{
public:
    ObjectCoordinateNode(IObject* object);
    ~ObjectCoordiateNode();

private:
    IObject* _object;
};
#endif
