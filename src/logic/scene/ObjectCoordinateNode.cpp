#include "ObjectCoordinateNode.h"
#include <set>
#include "IDCCenter.h"
#include <algorithm>
#include <iterator>
#include "Scene.h"

class CoordinateNodeWrapX{
public:
    CoordinateNodeWrapX(CoordinateNode* node, const Position3D& originPos)
        :_pNode(node),
         _pCurrentNode(node),
         _originPos(originPos)
    {}

    inline void reset() { _pCurrentNode = _pNode;}
    inline bool isObjectNode() const {return _pCurrentNode->hasFlags(CoordinateNode::COORDINATE_NODE_FLAG_OBJECT);}
    inline bool valid() const {return !_pCurrentNode->hasFlags(CoordinateNode::COORDINATE_NODE_FLAG_REMOVED);}
    inline CoordinateNode* currentNode() const {return _pCurrentNode;}
    inline IObject* currentNodeObject() const {return static_cast<ObjectCoordinateNode*>(_pCurrentNode)->object();}

    inline CoordinateNode* prev(){
        _pCurrentNode = _pCurrentNode->prevX();
        return _pCurrentNode;
    }

    inline CoordinateNode* next(){
        _pCurrentNode = _pCurrentNode->nextX();
        return _pCurrentNode;
    }

    inline int compare(){
        //float v = currentNodeObject()
        return 1;
    }

    inline float length(){
        return 0;
    }

protected:
    CoordinateNode* _pNode;
    CoordinateNode* _pCurrentNode;
    const Position3D& _originPos;
};

class CoordinateNodeWrapZ: public CoordinateNodeWrapX{
public:
    CoordinateNodeWrapZ(CoordinateNode* pNode, const Position3D& originPos)
        :CoordinateNodeWrapX(pNode, originPos)
    {}

    inline CoordinateNode* prev(){
        _pCurrentNode = _pCurrentNode->prevZ();
        return _pCurrentNode;
    }

    inline CoordinateNode* next(){
        _pCurrentNode = _pCurrentNode->nextZ();
        return _pCurrentNode;
    }

    inline int compare(){
        return 1;
    }

    inline float length(){
        return 0;
    }

};

class CoordinateNodeWrapY: public CoordinateNodeWrapX{
public:
    CoordinateNodeWrapY(CoordinateNode* pNode, const Position3D& originPos)
        :CoordinateNodeWrapX(pNode, originPos)
    {}

    inline CoordinateNode* prev(){
        _pCurrentNode = _pCurrentNode->prevY();
        return _pCurrentNode;
    }

    inline CoordinateNode* next(){
        _pCurrentNode = _pCurrentNode->nextY();
        return _pCurrentNode;
    }

    inline int compare(){
        return 0;
    }

    inline float length(){
        return 0;
    }
};

template <class NODEWRAP>
CoordinateNode* findNearestNode(CoordinateNode* rootNode, const Position3D& originPos){
    CoordinateNode* pRN = NULL;
    CoordinateNode* pCoordinateNode = rootNode;

    //find base node
    {
        //find to left
        NODEWRAP wrap(rootNode, originPos);
        do{
            if(wrap.isObjectNode() && wrap.valid()){
                pRN = wrap.currentNode();
                break;
            }
        }while(wrap.prev());
        
        //find to right
        if(!pRN){
            wrap.reset();
            while(wrap.next()){
                if(wrap.isObjectNode() && wrap.valid()){
                    pRN = wrap.currentNode();
                    break;
                }
            }

            if(!pRN)
                return NULL;
        }
    }

    NODEWRAP wrap(pRN, originPos);
   
    int v = wrap.compare();
    if(v == 0){
        return wrap.currentNode();
    }
    else if(v > 0) //at right
    {
        pCoordinateNode = wrap.currentNode();
        while(wrap.prev()){
            if(wrap.isObjectNode() && wrap.valid()){
                if(wrap.compare() <= 0)
                    return wrap.currentNode();
            }
            pCoordinateNode = wrap.currentNode();
        }
        
        return pCoordinateNode;
    }
    else    //at left
    {
        pCoordinateNode = wrap.currentNode();
        while(wrap.next()){
            if(wrap.isObjectNode() && wrap.valid()){
                if(wrap.compare() >= 0)
                    return wrap.currentNode();
            }
            pCoordinateNode = wrap.currentNode();
        }

        return pCoordinateNode;
    }
}

template <class NODEWRAP>
void objectsInAxisRange(std::set<IObject*>& foundObjects, CoordinateNode* rootNode, const Position3D& originPos, float radius, int objectType){
    CoordinateNode* pCoordinateNode = findNearestNode<NODEWRAP>(rootNode, originPos);
    if(!pCoordinateNode)
        return;

    NODEWRAP wrap(pCoordinateNode, originPos);

    if(wrap.isObjectNode() && wrap.valid()){
        IObject* object = wrap.currentNodeObject();
        if(objectType == -1 || object->getObjectType() == objectType){
            if(wrap.length() <= radius)
                foundObjects.insert(object);
        }
    }

    while(wrap.prev()){
        if(wrap.isObjectNode() && wrap.valid()){
            IObject* object = wrap.currentNodeObject();
            if(objectType == -1 || object->getObjectType() == objectType){
                if(wrap.length() <= radius)
                    foundObjects.insert(object);
                else
                    break;
            }
        }
    };

    wrap.reset();

    while(wrap.next()){
        if(wrap.isObjectNode() && wrap.valid()){
            IObject * object = wrap.currentNodeObject();
            if(objectType == -1 || object->getObjectType() == objectType){
                if(wrap.length() <= radius)
                    foundObjects.insert(object);
                else
                    break;
            }
        }
    };
}

ObjectCoordinateNode::ObjectCoordinateNode(IObject* object)
    :CoordinateNode(NULL),
     _object(object),
     _objectNodeUpdating(0),
     _delWatcherNodeNum(0)
{
    setFlags(CoordinateNode::COORDINATE_NODE_FLAG_OBJECT);
}

ObjectCoordinateNode::~ObjectCoordinateNode(){
    _watcherNodes.clear();
}

float ObjectCoordinateNode::xx() const{
    if(_object == NULL || hasFlags((CoordinateNode::COORDINATE_NODE_FLAG_REMOVED | CoordinateNode::COORDINATE_NODE_FLAG_REMOVING)))
        return -FLT_MAX;

    return _object->getPropFloat(Scene::getInstance()->getPropX());
}

float ObjectCoordinateNode::yy() const{
    if(_object == NULL || hasFlags((CoordinateNode::COORDINATE_NODE_FLAG_REMOVED | CoordinateNode::COORDINATE_NODE_FLAG_REMOVING)))
        return -FLT_MAX;

    return _object->getPropFloat(Scene::getInstance()->getPropY());
}

float ObjectCoordinateNode::zz() const{
    if(_object == NULL || hasFlags((CoordinateNode::COORDINATE_NODE_FLAG_REMOVED | CoordinateNode::COORDINATE_NODE_FLAG_REMOVING)))
        return -FLT_MAX;

    return _object->getPropFloat(Scene::getInstance()->getPropZ());
}


void ObjectCoordinateNode::update(){
    setOldX(x());
    setOldY(y());
    setOldZ(z());
    
    CoordinateNode::update();
    
    addFlags(CoordinateNode::COORDINATE_NODE_FLAG_NODE_UPDATING);
    ++_objectNodeUpdating;

    for(WATCHER_NODES::size_type i = 0; i < _watcherNodes.size(); ++i){
        CoordinateNode* pCoordinateNode = _watcherNodes[i];
        if(!pCoordinateNode)
            continue;

        pCoordinateNode->update();
    }

    --_objectNodeUpdating;
    if(_objectNodeUpdating == 0)
        removeFlags(CoordinateNode::COORDINATE_NODE_FLAG_NODE_UPDATING);

}

void ObjectCoordinateNode::clearDelWatcherNodes(){
    if(hasFlags((CoordinateNode::COORDINATE_NODE_FLAG_NODE_UPDATING | CoordinateNode::COORDINATE_NODE_FLAG_REMOVING | CoordinateNode::COORDINATE_NODE_FLAG_REMOVED)))
        return;

    if(_delWatcherNodeNum > 0){
        WATCHER_NODES::iterator iter = _watcherNodes.begin();
        for(; iter != _watcherNodes.end();){
            if(!(*iter)){
                iter = _watcherNodes.erase(iter);
                --_delWatcherNodeNum;

                if(_delWatcherNodeNum <= 0)
                    return;
            }
            else{
                ++iter;
            }
        }
    }
}

void ObjectCoordinateNode::onRemove(){
    for(WATCHER_NODES::size_type i = 0; i < _watcherNodes.size(); ++i){
        CoordinateNode* pCoordinateNode = _watcherNodes[i];

        if(!pCoordinateNode)
            continue;

        _watcherNodes[i] = NULL;
        ++_delWatcherNodeNum;
    }

    CoordinateNode::onRemove();
}

bool ObjectCoordinateNode::addWatcherNode(CoordinateNode* pNode){
    clearDelWatcherNodes();
    
    WATCHER_NODES::iterator iter = std::find(_watcherNodes.begin(), _watcherNodes.end(), pNode);
    if(iter != _watcherNodes.end())
        return false;

    _watcherNodes.push_back(pNode);
    
    onAddWatcherNode(pNode);
    return true;
}

void ObjectCoordinateNode::onAddWatcherNode(CoordinateNode* pNode){
}

bool ObjectCoordinateNode::delWatcherNode(CoordinateNode* pNode){
    WATCHER_NODES::iterator iter = std::find(_watcherNodes.begin(), _watcherNodes.end(), pNode);
    if(iter == _watcherNodes.end())
        return false;

    if(hasFlags((CoordinateNode::COORDINATE_NODE_FLAG_NODE_UPDATING | CoordinateNode::COORDINATE_NODE_FLAG_REMOVING | CoordinateNode::COORDINATE_NODE_FLAG_REMOVED))){
        (*iter) = NULL;
        ++_delWatcherNodeNum;
    }
    else{
        _watcherNodes.erase(iter);
    }

    return true;
}


void ObjectCoordinateNode::objectsInRange(std::vector<IObject*>& foundObjects, CoordinateNode* rootNode, const Position3D& originPos, float radius, int objectType){
    std::set<IObject*> objectsOnX;
    std::set<IObject*> objectsOnZ;

    objectsInAxisRange<CoordinateNodeWrapX>(objectsOnX, rootNode, originPos, radius, objectType);
    objectsInAxisRange<CoordinateNodeWrapZ>(objectsOnZ, rootNode, originPos, radius, objectType);

    std::set<IObject*> objectsOnY;
    objectsInAxisRange<CoordinateNodeWrapY>(objectsOnY, rootNode, originPos, radius, objectType);

    std::set<IObject*> res;
    set_intersection(objectsOnX.begin(), objectsOnX.end(), objectsOnZ.begin(), objectsOnZ.end(), std::inserter(res, res.end()));
    set_intersection(res.begin(), res.end(), objectsOnY.begin(), objectsOnY.end(), std::back_inserter(foundObjects));
}


