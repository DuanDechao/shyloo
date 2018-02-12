#ifndef __SL_FRAMEWORK_SCENE_H__
#define __SL_FRAMEWORK_SCENE_H__
#include "slsingleton.h"
#include "IScene.h"
class Scene : public IScene, public sl::SLHolder<Scene>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);
    
    inline const IProp* getPropX() const {return _propX;}
    inline const IProp* getPropY() const {return _propY;}
    inline const IProp* getPropZ() const {return _propZ;}
    inline const IProp* getPropCoordinateNode() const {return _propCoordinateNode;}

private:
    bool appendSceneProp();

private:
    const IProp* _propX;
    const IProp* _propY;
    const IProp* _propZ;
    const IProp* _propCoordinateNode;
};
#endif
