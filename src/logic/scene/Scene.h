#ifndef __SL_FRAMEWORK_SCENE_H__
#define __SL_FRAMEWORK_SCENE_H__
#include "slsingleton.h"
#include "IScene.h"
#include <map>
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
    inline const IProp* getPropRoll() const {return _propRoll;}
    inline const IProp* getPropPitch() const {return _propPitch;}
    inline const IProp* getPropYaw() const {return _propYaw;}
    inline const IProp* getPropCoordinateNode() const {return _propCoordinateNode;}
    inline const IProp* getPropSceneNodeData() const {return _propSceneNodeData;}
    inline const IProp* getPropWitness() const {return _propWitness;}

    
    virtual ISpace* createNewSpace(const uint32 spaceId);
    virtual ISpace* findSpace(const uint32 spaceId);
    virtual int32 spaceSize() {return (int32)_spaces.size();}
    virtual IWitness* getWitness(IObject* object);
    virtual void updatePosition(IObject* object, float x, float y, float z);
    virtual void getPosition(IObject* object, float& x, float& y, float& z);
    virtual void updateDirection(IObject* object, float roll, float pitch, float yaw);
    virtual void getDirection(IObject* object, float& roll, float& pitch, float& yaw);
    virtual int32 getSpaceId(IObject* object);

private:
    bool appendSceneProp();


private:
    static Scene* s_self;
    const IProp* _propX;
    const IProp* _propY;
    const IProp* _propZ;
	const IProp* _propRoll;
	const IProp* _propPitch;
	const IProp* _propYaw;
    const IProp* _propCoordinateNode;
    const IProp* _propSceneNodeData;
    const IProp* _propWitness;
    const IProp* _propSpaceId;
    std::map<uint32, ISpace*>    _spaces;
};
#endif
