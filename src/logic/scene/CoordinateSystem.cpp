#include "CoordinateSystem.h"
#include <float.h>
#include <algorithm>
CoordinateSystem::CoordinateSystem()
    :_xCoordinateHead(nullptr),
     _yCoordinateHead(nullptr),
     _zCoordinateHead(nullptr),
     _size(0),
     _delCount(0)
{}

CoordinateSystem::~CoordinateSystem(){}

bool CoordinateSystem::insert(CoordinateNode* pNode){
    if(isEmpty()){
        _xCoordinateHead = pNode;
        _yCoordinateHead = pNode;
        _zCoordinateHead = pNode;

        pNode->setPrevX(NULL);
        pNode->setPrevY(NULL);
        pNode->setPrevZ(NULL);
        pNode->setNextX(NULL);
        pNode->setNextY(NULL);
        pNode->setNextZ(NULL);

        pNode->setX(pNode->xx());
        pNode->setY(pNode->yy());
        pNode->setZ(pNode->zz());
        
        pNode->setCoordinateSystem(this);

        pNode->resetOld();

        return true;
    }

    pNode->setOldX(-FLT_MAX);
    pNode->setOldY(-FLT_MAX);
    pNode->setOldZ(-FLT_MAX);

    pNode->setX(_xCoordinateHead->x());
    _xCoordinateHead->setPrevX(pNode);
    pNode->setNextX(_xCoordinateHead);
    _xCoordinateHead = pNode;

    pNode->setY(_yCoordinateHead->y());
    _yCoordinateHead->setPrevY(pNode);
    pNode->setNextY(_yCoordinateHead);
    _yCoordinateHead = pNode;
    
    pNode->setZ(_zCoordinateHead->z());
    _zCoordinateHead->setPrevZ(pNode);
    pNode->setNextZ(_zCoordinateHead);
    _zCoordinateHead = pNode;

    pNode->setCoordinateSystem(this);
    ++_size;

    update(pNode);

    return true;
}

bool CoordinateSystem::remove(CoordinateNode* pNode){
    pNode->addFlags(CoordinateNode::COORDINATE_NODE_FLAG_REMOVING);
    pNode->onRemove();
    update(pNode);

    pNode->addFlags(CoordinateNode::COORDINATE_NODE_FLAG_REMOVED);
    
    std::list<CoordinateNode*>::iterator iter = std::find(_dels.begin(), _dels.end(), pNode);
    if(iter == _dels.end()){
        _dels.push_back(pNode);
        ++_delCount;
    }
    
    return true;
}

bool CoordinateSystem::removeReal(CoordinateNode* pNode){
    if(pNode->coordinateSystem() == NULL)
        return true;

    if(_xCoordinateHead == pNode){
        _xCoordinateHead = _xCoordinateHead->nextX();
        if(_xCoordinateHead)
            _xCoordinateHead->setPrevX(NULL);
    }else{
        pNode->prevX()->setNextX(pNode->nextX());
        
        if(pNode->nextX())
            pNode->nextX()->setPrevX(pNode->prevX());
    }

    if(_yCoordinateHead == pNode){
        _yCoordinateHead = _yCoordinateHead->nextY();
        if(_yCoordinateHead)
            _yCoordinateHead->setPrevY(NULL);
    }else{
        pNode->prevY()->setNextY(pNode->nextY());
        if(pNode->nextY())
            pNode->nextY()->setPrevY(pNode->prevY());
    }

    if(_zCoordinateHead == pNode){
        _zCoordinateHead = _zCoordinateHead->nextZ();
        if(_zCoordinateHead)
            _zCoordinateHead->setPrevZ(NULL);
    }else{
        pNode->prevZ()->setNextZ(pNode->nextZ());

        if(pNode->nextZ())
            pNode->nextZ()->setPrevZ(pNode->prevZ());
    }

    pNode->setPrevX(NULL);
    pNode->setPrevY(NULL);
    pNode->setPrevZ(NULL);
    pNode->setNextX(NULL);
    pNode->setNextY(NULL);
    pNode->setNextZ(NULL);
    pNode->setCoordinateSystem(NULL);
    
    _releases.push_back(pNode);

    --_size;

    return true;

}

void CoordinateSystem::removeDelNodes(){
    if(_delCount == 0)
        return;

    std::list<CoordinateNode*>::iterator iter = _dels.begin();
    std::list<CoordinateNode*>::iterator iterEnd = _dels.end();
    for(; iter != iterEnd; ++iter){
        removeReal((*iter));
    }

    _dels.clear();
    _delCount = 0;
}

void CoordinateSystem::releaseNodes(){
    removeDelNodes();
    std::list<CoordinateNode*>::iterator iter = _releases.begin();
    std::list<CoordinateNode*>::iterator iterEnd = _releases.end();

    for(;iter != iterEnd; ++iter){
        DEL (*iter);
    }

    _releases.clear();
    
}
void CoordinateSystem::debugNodes(){
    CoordinateNode* pNode = _xCoordinateHead;
/*    printf("-------------------------------x Coordinate debug start---------------------------------\n");
    while(pNode){
        printf("node %p: isLeftTriggerNode:%d isRightTriggerNode:%d isObjectNode:%d isRightTriggerNode:xx(%f) yy(%f) zz(%f) oldX(%f) oldY(%f) oldZ(%f) x(%f) y(%f) z(%f)\n", pNode, pNode->hasFlags(CoordinateNode::COORDINATE_NODE_FLAG_NEGATIVE_BOUNDARY), pNode->hasFlags(CoordinateNode::COORDINATE_NODE_FLAG_POSITIVE_BOUNDARY), pNode->hasFlags(CoordinateNode::COORDINATE_NODE_FLAG_OBJECT), pNode->xx(), pNode->yy(), pNode->zz(), pNode->oldX(), pNode->oldY(), pNode->oldZ(), pNode->x(), pNode->y(), pNode->z());
        pNode =pNode->nextX();
    }
    printf("-------------------------------x Coordinate debug end------------------------------\n");

   pNode = _yCoordinateHead;
    printf("-------------------------------y Coordinate debug start---------------------------------\n");
    while(pNode){
        printf("node %p: isLeftTriggerNode:%d isRightTriggerNode:%d isObjectNode:%d isRightTriggerNode:xx(%f) yy(%f) zz(%f) oldX(%f) oldY(%f) oldZ(%f) x(%f) y(%f) z(%f)\n", pNode, pNode->hasFlags(CoordinateNode::COORDINATE_NODE_FLAG_NEGATIVE_BOUNDARY), pNode->hasFlags(CoordinateNode::COORDINATE_NODE_FLAG_POSITIVE_BOUNDARY), pNode->hasFlags(CoordinateNode::COORDINATE_NODE_FLAG_OBJECT), pNode->xx(), pNode->yy(), pNode->zz(), pNode->oldX(), pNode->oldY(), pNode->oldZ(), pNode->x(), pNode->y(), pNode->z());
        pNode =pNode->nextY();
    }
    printf("-------------------------------y Coordinate debug end------------------------------\n");*/
    
   pNode = _zCoordinateHead;
    printf("-------------------------------z Coordinate debug start---------------------------------\n");
    while(pNode){
        printf("node %p: isLeftTriggerNode:%d isRightTriggerNode:%d isObjectNode:%d isRightTriggerNode:xx(%f) yy(%f) zz(%f) oldX(%f) oldY(%f) oldZ(%f) x(%f) y(%f) z(%f)\n", pNode, pNode->hasFlags(CoordinateNode::COORDINATE_NODE_FLAG_NEGATIVE_BOUNDARY), pNode->hasFlags(CoordinateNode::COORDINATE_NODE_FLAG_POSITIVE_BOUNDARY), pNode->hasFlags(CoordinateNode::COORDINATE_NODE_FLAG_OBJECT), pNode->xx(), pNode->yy(), pNode->zz(), pNode->oldX(), pNode->oldY(), pNode->oldZ(), pNode->x(), pNode->y(), pNode->z());

        pNode =pNode->nextZ();
    }
    printf("-------------------------------z Coordinate debug end------------------------------\n"); 
}

void CoordinateSystem::update(CoordinateNode* pNode){
    //printf("CoordindateNode[%p] starting update... old:%f %f %f curr:%f %f %f\n", pNode, pNode->oldX(), pNode->oldY(), pNode->oldZ(), pNode->xx(), pNode->yy(), pNode->zz());
    if(pNode->xx() != pNode->oldX()){
        while(true){
            CoordinateNode* pCurrNode = pNode->prevX();
            while(pCurrNode && pCurrNode != pNode && pCurrNode->x() > pNode->xx()){
                pNode->setX(pCurrNode->x());

                moveNodeX(pNode, pNode->xx(), pCurrNode);

                if(!pNode->hasFlags((CoordinateNode::COORDINATE_NODE_FLAG_HIDE | CoordinateNode::COORDINATE_NODE_FLAG_REMOVED)))
                    pCurrNode->onNodePassX(pNode);
                
                if(!pCurrNode->hasFlags(CoordinateNode::COORDINATE_NODE_FLAG_HIDE | CoordinateNode::COORDINATE_NODE_FLAG_REMOVED))
                    pNode->onNodePassX(pCurrNode);

                if(pNode->prevX() == NULL)
                    break;

                pCurrNode = pNode->prevX();
            }

            pCurrNode = pNode->nextX();
            while(pCurrNode && pCurrNode != pNode && pCurrNode->x() < pNode->xx()){
                pNode->setX(pCurrNode->x());

                moveNodeX(pNode, pNode->xx(), pCurrNode);
                
                if(!pNode->hasFlags((CoordinateNode::COORDINATE_NODE_FLAG_HIDE | CoordinateNode::COORDINATE_NODE_FLAG_REMOVED)))
                    pCurrNode->onNodePassX(pNode);
                
                if(!pCurrNode->hasFlags((CoordinateNode::COORDINATE_NODE_FLAG_HIDE | CoordinateNode::COORDINATE_NODE_FLAG_REMOVED)))
                    pNode->onNodePassX(pCurrNode);

                if(pNode->nextX() == NULL)
                    break;

                pCurrNode = pNode->nextX();
            }

            if((pNode->prevX() == NULL || (pNode->xx() >= pNode->prevX()->x())) &&
                    (pNode->nextX() == NULL || (pNode->xx() <= pNode->nextX()->x())))
            {
                pNode->setX(pNode->xx());
                break;
            }
        }
    }


    if(pNode->yy() != pNode->oldY()){
        while(true){
            CoordinateNode* pCurrNode = pNode->prevY();
            while(pCurrNode && pCurrNode != pNode && pCurrNode->y() > pNode->yy()){
                pNode->setY(pCurrNode->y());

                moveNodeY(pNode, pNode->yy(), pCurrNode);
                
                if(!pNode->hasFlags((CoordinateNode::COORDINATE_NODE_FLAG_HIDE | CoordinateNode::COORDINATE_NODE_FLAG_REMOVED)))
                    pCurrNode->onNodePassY(pNode);
                
                if(!pCurrNode->hasFlags((CoordinateNode::COORDINATE_NODE_FLAG_HIDE | CoordinateNode::COORDINATE_NODE_FLAG_REMOVED)))
                    pNode->onNodePassY(pCurrNode);

                if(pNode->prevY() == NULL)
                    break;

                pCurrNode = pNode->prevY();
            }

            pCurrNode = pNode->nextY();
            while(pCurrNode && pCurrNode != pNode && pCurrNode->y() < pNode->yy()){
                pNode->setY(pCurrNode->y());

                moveNodeY(pNode, pNode->yy(), pCurrNode);
                
                if(!pNode->hasFlags((CoordinateNode::COORDINATE_NODE_FLAG_HIDE | CoordinateNode::COORDINATE_NODE_FLAG_REMOVED)))
                    pCurrNode->onNodePassY(pNode);
                
                if(!pCurrNode->hasFlags((CoordinateNode::COORDINATE_NODE_FLAG_HIDE | CoordinateNode::COORDINATE_NODE_FLAG_REMOVED)))
                    pNode->onNodePassY(pCurrNode);

                if(pNode->nextY() == NULL)
                    break;

                pCurrNode = pNode->nextY();
            }

            if((pNode->prevY() == NULL || (pNode->yy() >= pNode->prevY()->y())) &&
                    (pNode->nextY() == NULL || (pNode->yy() <= pNode->nextY()->y())))
            {
                pNode->setY(pNode->yy());
                break;
            }
        }
    }

    if(pNode->zz() != pNode->oldZ()){
        while(true){
            CoordinateNode* pCurrNode = pNode->prevZ();
            while(pCurrNode && pCurrNode != pNode && pCurrNode->z() > pNode->zz()){
                pNode->setZ(pCurrNode->z());

                moveNodeZ(pNode, pNode->zz(), pCurrNode);
                
                if(!pNode->hasFlags((CoordinateNode::COORDINATE_NODE_FLAG_HIDE | CoordinateNode::COORDINATE_NODE_FLAG_REMOVED)))
                    pCurrNode->onNodePassZ(pNode);
                
                if(!pCurrNode->hasFlags((CoordinateNode::COORDINATE_NODE_FLAG_HIDE | CoordinateNode::COORDINATE_NODE_FLAG_REMOVED)))
                    pNode->onNodePassZ(pCurrNode);

                if(pNode->prevZ() == NULL)
                    break;
                
                pCurrNode = pNode->prevZ();
            }

            pCurrNode = pNode->nextZ();
            while(pCurrNode && pCurrNode != pNode && pCurrNode->z() < pNode->zz()){
                pNode->setZ(pCurrNode->z());

                moveNodeZ(pNode, pNode->zz(), pCurrNode);
                
                if(!pNode->hasFlags((CoordinateNode::COORDINATE_NODE_FLAG_HIDE | CoordinateNode::COORDINATE_NODE_FLAG_REMOVED)))
                    pCurrNode->onNodePassZ(pNode);
                
                if(!pCurrNode->hasFlags((CoordinateNode::COORDINATE_NODE_FLAG_HIDE | CoordinateNode::COORDINATE_NODE_FLAG_REMOVED)))
                    pNode->onNodePassZ(pCurrNode);

                if(pNode->nextZ() == NULL)
                    break;

                pCurrNode = pNode->nextZ();
            }

            if((pNode->prevZ() == NULL || (pNode->zz() >= pNode->prevZ()->z())) &&
                    (pNode->nextZ() == NULL || (pNode->zz() <= pNode->nextZ()->z())))
            {
                pNode->setZ(pNode->zz());
                break;
            }
        }
    }

    pNode->resetOld();
}

void CoordinateSystem::moveNodeX(CoordinateNode* pNode, float px, CoordinateNode* pCurrNode){
    if(!pCurrNode)
        return;

    if(pCurrNode->x() > px){
        CoordinateNode* pPreNode = pCurrNode->prevX();
        pCurrNode->setPrevX(pNode);
        if(pPreNode){
            pPreNode->setNextX(pNode);
            if(pNode == _xCoordinateHead && pNode->nextX())
                _xCoordinateHead = pNode->nextX();
        }
        else{
            _xCoordinateHead = pNode;
        }

        if(pNode->prevX())
            pNode->prevX()->setNextX(pNode->nextX());

        if(pNode->nextX())
            pNode->nextX()->setPrevX(pNode->prevX());

        pNode->setPrevX(pPreNode);
        pNode->setNextX(pCurrNode);
    }
    else{
        CoordinateNode* pNextNode = pCurrNode->nextX();
        if(pNextNode != pNode){
            pCurrNode->setNextX(pNode);
            if(pNextNode)
                pNextNode->setPrevX(pNode);

            if(pNode->prevX())
                pNode->prevX()->setNextX(pNode->nextX());

            if(pNode->nextX()){
                pNode->nextX()->setPrevX(pNode->prevX());

                if(pNode == _xCoordinateHead)
                    _xCoordinateHead = pNode->nextX();
            }

            pNode->setPrevX(pCurrNode);
            pNode->setNextX(pNextNode);
        }
    }
}

void CoordinateSystem::moveNodeY(CoordinateNode* pNode, float py, CoordinateNode* pCurrNode){
    if(!pCurrNode)
        return;

    if(pCurrNode->y() > py){
        CoordinateNode* pPreNode = pCurrNode->prevY();
        pCurrNode->setPrevY(pNode);
        if(pPreNode){
            pPreNode->setNextY(pNode);
            if(pNode == _yCoordinateHead && pNode->nextY())
                _yCoordinateHead = pNode->nextY();
        }
        else{
            _yCoordinateHead = pNode;
        }

        if(pNode->prevY())
            pNode->prevY()->setNextY(pNode->nextY());

        if(pNode->nextY())
            pNode->nextY()->setPrevY(pNode->prevY());

        pNode->setPrevY(pPreNode);
        pNode->setNextY(pCurrNode);
    }
    else{
        CoordinateNode* pNextNode = pCurrNode->nextY();
        if(pNextNode != pNode){
            pCurrNode->setNextY(pNode);
            if(pNextNode)
                pNextNode->setPrevY(pNode);

            if(pNode->prevY())
                pNode->prevY()->setNextY(pNode->nextY());

            if(pNode->nextY()){
                pNode->nextY()->setPrevY(pNode->prevY());
                
                if(pNode == _yCoordinateHead)
                    _yCoordinateHead = pNode->nextY();
            }

            pNode->setPrevY(pCurrNode);
            pNode->setNextY(pNextNode);
        }
    }
}

void CoordinateSystem::moveNodeZ(CoordinateNode* pNode, float pz, CoordinateNode* pCurrNode){
    if(!pCurrNode)
        return;

    if(pCurrNode->z() > pz){
        CoordinateNode* pPreNode = pCurrNode->prevZ();
        pCurrNode->setPrevZ(pNode);
        if(pPreNode){
            pPreNode->setNextZ(pNode);
            if(pNode == _zCoordinateHead && pNode->nextZ())
                _zCoordinateHead = pNode->nextZ();
        }
        else{
            _zCoordinateHead = pNode;
        }

        if(pNode->prevZ())
            pNode->prevZ()->setNextZ(pNode->nextZ());

        if(pNode->nextZ())
            pNode->nextZ()->setPrevZ(pNode->prevZ());

        pNode->setPrevZ(pPreNode);
        pNode->setNextZ(pCurrNode);
    }
    else{
        CoordinateNode* pNextNode = pCurrNode->nextZ();
        if(pNextNode != pNode){
            pCurrNode->setNextZ(pNode);
            if(pNextNode)
                pNextNode->setPrevZ(pNode);

            if(pNode->prevZ())
                pNode->prevZ()->setNextZ(pNode->nextZ());

            if(pNode->nextZ()){
                pNode->nextZ()->setPrevZ(pNode->prevZ());

                if(pNode == _zCoordinateHead)
                    _zCoordinateHead = pNode->nextZ();
            }

            pNode->setPrevZ(pCurrNode);
            pNode->setNextZ(pNextNode);
        }
    }
}
