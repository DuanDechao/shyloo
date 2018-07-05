#ifndef _SL_INTERFACE_SCENE_H__
#define _SL_INTERFACE_SCENE_H__
#include "slimodule.h"
#include "slbinary_stream.h"
#include <map>
class IObject;

class IWitness{
public:
    virtual ~IWitness() {}
};

class ISpace{
public:
    virtual ~ISpace() {}
    virtual void addObject(IObject* object) = 0;
    virtual void addObjectToNode(IObject* object) = 0;
    virtual void addSpaceDataToStream(sl::IBStream& stream) = 0;
    virtual void onEnterWorld(IObject* object) = 0;
    virtual IWitness* setWitness(IObject* object) = 0;
	virtual bool addSpaceGeometryMapping(const char* path, bool shouldLoadOnServer, const std::map<int32, std::string> params) = 0;
	virtual int32 getId() const = 0;
};


class IScene : public sl::api::IModule{
public:
	virtual ~IScene() {}
    virtual ISpace* createNewSpace(const uint32 spaceId) = 0;
    virtual ISpace* findSpace(const uint32 spaceId) = 0;
    virtual IWitness* getWitness(IObject* object) = 0;
    virtual void updatePosition(IObject* object, float x, float y, float z) = 0;
    virtual void getPosition(IObject* object, float& x, float& y, float& z) = 0;
    virtual void updateDirection(IObject* object, float roll, float pitch, float yaw) = 0;
    virtual void getDirection(IObject* object, float& roll, float& pitch, float& yaw) = 0;
    virtual int32 getSpaceId(IObject* object) = 0;
};

#endif
