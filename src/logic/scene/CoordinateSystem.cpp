#include "CoordinateSystem.h"
CoordinateSystem::CoordinateSystem()
    :_xCoordinateHead(nullptr),
     _yCoordinateHead(nullptr),
     _zCoordinateHead(nullptr)
{}

CoordiateSystem::~CoordinateSystem(){}

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

    pNode->setCoordinateSystem(this);

    update(pNode);

    return true;
}

void CoordinateSystem::update(CoordinateNode* pNode){
    if(pNode->xx() != pNode->oldX()){
        while(true){
            CoordinateNode* pCurrNode = pNode->pPrevX();
            while(pCurrNode && pCurrNode != pNode && && pCurrNode->x() > pNode->xx()){
                pNode->setX(pCurrNode->x());

                moveNodeX(pNode, pNode->xx(), pCurrNode);

                //if(!pNode->)
                pCurrNode->onNodePassX(pNode, true);

                pNode->onNodePassX(pCurrNode, false);
            }
        }
    }
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
