#ifndef __SL_FRAMEWORK_SCENE_NODE_DATA_H__
#define __SL_FRAMEWORK_SCENE_NODE_DATA_H__
#include "slmulti_sys.h"
#include <vector>
#include <list>
#include "IDCCenter.h"

class IObject;
class SceneNodeData{
public:
    SceneNodeData(IObject* object);
    ~SceneNodeData();

    inline static SceneNodeData* create(IObject* object) {
        SceneNodeData* nodeData =  NEW SceneNodeData(object);
        object->setPropInt64(_objectPropSceneData, (int64)nodeData);
        return nodeData;
    }

    inline IObject* object() const {return _object;}
    inline void setObject(IObject* object) {_object = object;}
    

    void addWitnessed(IObject* object);
    void delWitnessed(IObject* object);

    void release(){
        DEL this;
    }

    inline static void addWitnessed(IObject* object, IObject* witness){
        SceneNodeData* nodeData = (SceneNodeData*)object->getPropInt64(_objectPropSceneData);
        if(nodeData)
            nodeData->addWitnessed(witness);
    }

    inline static void delWitnessed(IObject* object, IObject* witness){
        SceneNodeData* nodeData = (SceneNodeData*)object->getPropInt64(_objectPropSceneData);
        if(nodeData)
            nodeData->delWitnessed(witness);
    }

private:
    uint64                   _id;
    IObject*                _object;
    std::list<uint64>       _witnesses;
    int32                   _witnessesCount;
    static const IProp*     _objectPropSceneData;

};
#endif
