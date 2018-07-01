#ifndef __SL_FRAMEWORK_OBJECTCOORDINATENODE_H__
#define __SL_FRAMEWORK_OBJECTCOORDINATENODE_H__
#include "CoordinateNode.h"
#include <vector>
#include "slmath.h"
class IObject;
class ObjectCoordinateNode: public CoordinateNode{
public:
    ObjectCoordinateNode(IObject* object);



    virtual ~ObjectCoordinateNode();

    virtual float xx() const;
    virtual float yy() const;
    virtual float zz() const;

    virtual void update();

    IObject* object() const {return _object;}
    void setObject(IObject* object) {_object = object;}

    bool addWatcherNode(CoordinateNode* pNode);
    void onAddWatcherNode(CoordinateNode* pNode);

    bool delWatcherNode(CoordinateNode* pNode);

    static void objectsInRange(std::vector<IObject*>& foundObjects, CoordinateNode* rootNode,
            const Position3D& originPos, float radius, int objectType = -1);

    virtual void onRemove();

protected:
    void clearDelWatcherNodes();


protected:
    typedef std::vector<CoordinateNode*> WATCHER_NODES;
    IObject* _object;
    WATCHER_NODES _watcherNodes;
    
    int32 _objectNodeUpdating;
    int32 _delWatcherNodeNum;

};
#endif
