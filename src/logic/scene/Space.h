#ifndef __SL_FRAMEWORK_SPACE_H__
#define __SL_FRAMEWORK_SPACE_H__
#include <vector>
#include "slmulti_sys.h"
#include "CoordinateSystem.h"
#include "IScene.h"
#include <unordered_map>
class IObject;
class IProp;
class Space: public ISpace{
public:
    Space(const int32 spaceId);
    ~Space();

    virtual void addObject(IObject* object);

    virtual void addObjectToNode(IObject* object);

    //一个object进入了游戏世界
    virtual void onEnterWorld(IObject* object);


    //spaceData相关接口
    void setSpaceData(const std::string& key, const std::string& value);
    void delSpaceData(const std::string& key);
    bool hasSpaceData(const std::string& key);
    const std::string& getSpaceData(const std::string& key);
	void onSpaceDataChanged(const std::string& key, const std::string& value, bool del);
    virtual void addSpaceDataToStream(sl::IBStream& stream);
    virtual IWitness* setWitness(IObject* object);
	virtual bool addSpaceGeometryMapping(const char* path, bool shouldLoadOnServer, const std::map<int32, std::string> params);
    
    void positionChanged(sl::api::IKernel* pKernel, IObject* object, const char* name, const IProp* prop, const bool sync);

    static void setPropCoordinateNode(const IProp* pNodeProp) {_propCoordinateNode = pNodeProp;}
    static void setPropWitness(const IProp* pProp) { _propWitness = pProp;}
    static void setPropPosZ(const IProp* pProp) { _propPosZ = pProp;}
    static void setPropSpaceId(const IProp* pProp) { _propSpaceId = pProp;}

private:
	void setGeometryPath(std::string& path);
	const std::string& getGeometryPath();
	void loadSpaceGeometry(const std::map<int32, std::string>& params){}

private:
    int32                   _id;
    std::vector<IObject*>   _spaceObjects;
    CoordinateSystem        _coordinateSystem;
    std::unordered_map<std::string, std::string> _spaceDatas;
	bool					_hasGeometry;
    static const IProp*     _propCoordinateNode;
    static const IProp*     _propWitness;
    static const IProp*     _propPosZ;
    static const IProp*     _propSpaceId;
};
#endif
