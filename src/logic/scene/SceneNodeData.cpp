#include "SceneNodeData.h"
SceneNodeData::SceneNodeData(IObject* object)
    :_id(object->getID()),
     _object(object),
     _witnessesCount(0)
{}

SceneNodeData::~SceneNodeData(){
}


void SceneNodeData::addWitnessed(IObject* object){
    _witnesses.push_back(object->getID());
    ++_witnessesCount;
}

void SceneNodeData::delWitnessed(IObject* object){
    _witnesses.remove(object->getID());
    --_witnessesCount;
}
