#include "Witness.h"

Witness::Witness()
    :_object(NULL),
     _aoiRadius(0.0f),
     _aoiTrigger(NULL),
     _clientAOISize(0)
{}

Witness::~Witness(){
    _object = NULL;
    if(_aoiTrigger)
        DEL _aoiTrigger;
    _aoiTrigger = NULL;
}

void Witness::attach(IObject* object){
    _object = object;
    _lastBasePos.z = -FLT_MAX;
    _lastBaseDir.yaw(-FLT_MAX);

    setAoiRadius(50);

    onAttach(object);
}

void Witness::onAttach(IObject* object){
    _lastBasePos.z = -FLT_MAX;
    _lastBaseDir.yaw(-FLT_MAX);
}

void Witness::detach(IObject* object){
    clear(object);
}

void Witness::clear(IObject* object){
    uninstallAOITrigger();

    AOI_OBJECTS::iterator iter = _aoiObjects.begin();
    AOI_OBJECTS::iterator iterEnd = _aoiObjects.end();
    for(; iter != iterEnd; ++iter){
        if((*iter)->object()){
            //(*iter)->object()->delWitnessed
        }
    }
}

void Witness::setAoiRadius(float radius){
    _aoiRadius = radius;

    if(_aoiRadius > 0.f && _object){
        if(_aoiTrigger == NULL){
            CoordinateNode* pObjectNode = (CoordinateNode*)_object->getPropInt64(Scene::getInstance()->getPropCoordinateNode());
            _aoiTrigger = NEW AOITrigger(pObjectNode, _aoiRadius, _aoiRadius);

            if(pObjectNode->coordinateSystem())
                _aoiTrigger->install();
        }
        else{
            _aoiTrigger->update(_aoiRadius, _aoiRadius);

            if(!_aoiTrigger->isInstalled() && pObjectNode->coordinateSystem())
                _aoiTrigger->reinstall(pObjectNode);
        }
    }
    else{
        uninstallAOITrigger();
    }
}


void Witness::onEnterAOI(AOITrigger* pAOITrigger, IObject* object){
    AOI_OBJECTS_MAP::iterator iter = _aoiObjectsMap.find(object->getID());
    if(iter != _aoiObjectsMap.end()){
        ObjectRef* pObjectRef = iter->second;
        if((pObjectRef->flags() & ObjectRef::OBJECTREF_FLAG_LEAVE_CLIENT_PENDING) > 0){
            if((pObjectRef->flags() & ObjectRef::OBJECTREF_FLAG_NORMAL) > 0)
                pObjectRef->setFlags(ObjectRef::OBJECTREF_FLAG_NORMAL);
            else
                pObjectRef->setFlags(ObjectRef::OBJECTREF_FLAG_ENTER_CLIENT_PENDING);

            pObjectRef->setObject(object);
        }    
    }

    ObjectRef* pObjectRef = ObjectRef::create(object);
    pObjectRef->setFlags(pObjectRef->flags() | ObjectRef::OBJECTREF_FLAG_ENTER_CLIENT_PENDING);
    _aoiObjects.push_back(pObjectRef);
    _aoiObjectsMap[pObjectRef->id()] = pObjectRef;
}

void Witness::onLeaveAOI(AOITrigger* pAOITrigger, IObject* object){
    AOI_OBJECTS_MAP::iterator iter = _aoiObjectsMap.find(object->getID());
    if(iter == _aoiObjectsMap.end())
        return;

    _onLeaveAOI(iter->second);
}

void Witness::_onLeaveAOI(ObjectRef* pObjectRef){
    pObjectRef->setFlags(((pObjectRef->flags() | ObjectRef::OBJECTREF_FLAG_LEAVE_CLIENT_PENDING) & ~(ObjectRef::OBJECTREF_FLAG_ENTER_CLIENT_PENDING)));

    if(pObjectRef->object())
       // pObjectRef->object()->

    pObjectRef->setObject(NULL);
}

void Witness::installAOITrigger(){
    if(_aoiTrigger)
        _aoiTrigger->uninstall();

    AOI_OBJECTS::iterator iter = _aoiObjects.begin();
    AOI_OBJECTS::iterator iterEnd = _aoiObjects.end();
    for(; iter != iterEnd; ++iter)
        _onLeaveAOI((*iter));
}
