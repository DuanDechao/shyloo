#ifndef __SL_FRAMEWORK_SCENE_H__
#define __SL_FRAMEWORK_SCENE_H__
#include "slsingleton.h"
#include "IScene.h"
class IProp;
class IObject;
class Scene : public IScene, public sl::SLHolder<Scene>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);
    
    inline const IProp* getPropX() const {return _propX;}
    inline const IProp* getPropY() const {return _propY;}
    inline const IProp* getPropZ() const {return _propZ;}
    inline const IProp* getPropCoordinateNode() const {return _propCoordinateNode;}
    inline const IProp* getPropSceneNodeData() const {return _propSceneNodeData;}

    void positionChanged(sl::api::IKernel* pKernel, IObject* object, const char* name, const IProp* prop, const bool sync);
    void test();
    
    void timerStart(sl::api::IKernel* pKernel, IObject* object, int64 tick);
    void onTimer(sl::api::IKernel* pKernel, IObject* object, int64 tick);
    void timerEnd(sl::api::IKernel* pKernel, IObject* object, bool novolient, int64 tick);

private:
    bool appendSceneProp();


private:
    static Scene* s_self;
    const IProp* _propX;
    const IProp* _propY;
    const IProp* _propZ;
    const IProp* _propCoordinateNode;
    const IProp* _propSceneNodeData;
    const IProp* _propPosChangeTimer;
};
#endif
