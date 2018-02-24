#include "SceneNodeData.h"
#include "Scene.h"
const IProp* SceneNodeData::_objectPropSceneData = NULL;
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
    printf("witness has:");
    for(auto witness : _witnesses)
        printf("%lld,", witness);

    printf("\n");
}

void SceneNodeData::delWitnessed(IObject* object){
    _witnesses.remove(object->getID());
    --_witnessesCount;
}
