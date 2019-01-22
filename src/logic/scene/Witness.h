#ifndef __SL_FRAMEWORK_WITNESS_H__
#define __SL_FRAMEWORK_WITNESS_H__
#include "slmath.h"
#include "IDCCenter.h"
#include "Scene.h"
#include <list>
#include <map>
#include "IScene.h"
class AOITrigger;
class Space;
class ObjectRef{
public:
    enum {
        OBJECTREF_FLAG_UNKNOWN                  =       0x00000000,
        OBJECTREF_FLAG_ENTER_CLIENT_PENDING     =       0x00000001,
        OBJECTREF_FLAG_LEAVE_CLIENT_PENDING     =       0x00000002,
        OBJECTREF_FLAG_NORMAL                   =       0x00000004,    
    };

    ObjectRef(IObject* object)
        :_id(object->getID()),
         _object(object),
         _flags(OBJECTREF_FLAG_UNKNOWN)
    {}

    ~ObjectRef(){}
    
    inline static ObjectRef* create(IObject* object){
        return NEW ObjectRef(object);
    }

    void release(){
        DEL this;
    }
    
    inline uint64 id() const {return _id;}

    inline void setFlags(uint32 v) {_flags = v;}
    inline void removeFlags(uint32 v) {_flags &= ~v;}
    inline uint32 flags() const {return _flags;}

    inline IObject* object() const {return _object;}
    inline void setObject(IObject* object) {_object = object;} 

private:
    uint64      _id;
    IObject*    _object;
    uint32      _flags; 
};

<<<<<<< HEAD
class Witness{
=======
class Witness: public IWitness{
>>>>>>> shipping
public:
    typedef std::list<ObjectRef*> AOI_OBJECTS;
    typedef std::map<uint64, ObjectRef*> AOI_OBJECTS_MAP;

    Witness();
    virtual ~Witness();

    inline IObject* getObject() {return _object;}
    inline void setObject(IObject* object) {_object = object;}

    void attach(IObject* object);
    void detach(IObject* object);
    void clear(IObject* object);
    void onAttach(IObject* object);
    
    void setAoiRadius(float radius);

    inline float aoiRadius() const {return _aoiRadius;}
    
    inline const Position3D& basePos() {
        _basePos.x = _object->getPropFloat(Scene::getInstance()->getPropX());
        _basePos.y = _object->getPropFloat(Scene::getInstance()->getPropY());
        _basePos.z = _object->getPropFloat(Scene::getInstance()->getPropZ());
        return _basePos;
    }

    inline const Direction3D& baseDir() {return _lastBaseDir;}

    bool update();

    void onEnterSpace(Space* pSpace);
    void onLeaveSpace(Space* pSpace);

    void onEnterAOI(AOITrigger* pAOITrigger, IObject* object);
    void onLeaveAOI(AOITrigger* pAOITrigger, IObject* object);
    void _onLeaveAOI(ObjectRef* pObjectRef);

    void installAOITrigger();
    void uninstallAOITrigger();

private:
    IObject*        _object;
    float           _aoiRadius;
    AOITrigger*     _aoiTrigger;
    AOI_OBJECTS     _aoiObjects;
    AOI_OBJECTS_MAP _aoiObjectsMap;

    Position3D      _lastBasePos;
    Direction3D     _lastBaseDir;

    Position3D      _basePos;
    Direction3D     _baseDir;

    int32           _clientAOISize;

};
#endif
