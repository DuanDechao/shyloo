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
    printf("++++++++object[%lld] add witness+++++++++++\n", _object->getID());
    for(auto witness : _witnesses)
        printf("%lld\n,", witness);

    printf("+++++++++++++++++++++++++++++++++++++++\n");
}

void SceneNodeData::delWitnessed(IObject* object){
    _witnesses.remove(object->getID());
    --_witnessesCount;
    printf("++++++++object[%lld] del witness+++++++++++\n", _object->getID());
    for(auto witness : _witnesses)
        printf("%lld\n,", witness);

    printf("+++++++++++++++++++++++++++++++++++++++\n");
}
