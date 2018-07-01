#include "Space.h"
#include "ObjectCoordinateNode.h"
#include "IDCCenter.h"
#include "Scene.h"
#include "Witness.h"
#include "INavigation.h"
const IProp* Space::_propCoordinateNode = NULL;
const IProp* Space::_propWitness = NULL;
const IProp* Space::_propPosZ = NULL;
const IProp* Space::_propSpaceId = NULL;
Space::Space(const int32 spaceId)
    :_id(spaceId),
	 _hasGeometry(false)
{}

Space::~Space(){}


void Space::addObject(IObject* object){
    if(!object->getPropInt64(_propCoordinateNode)){
        ObjectCoordinateNode* pCoordianteNode = NEW ObjectCoordinateNode(object);
        object->setPropInt64(_propCoordinateNode, (int64)pCoordianteNode);
    }

    RGS_PROP_CHANGER(object, _propPosZ, Space::positionChanged);
    _spaceObjects.push_back(object);
    object->setPropInt32(_propSpaceId, _id);    
}

void Space::addObjectToNode(IObject* object){
    ObjectCoordinateNode* pCoordinateNode = (ObjectCoordinateNode*)object->getPropInt64(_propCoordinateNode);
    _coordinateSystem.insert(pCoordinateNode); 
}

void Space::onEnterWorld(IObject* object){
   Witness* pWitness = (Witness*)object->getPropInt64(_propWitness);
   if(pWitness){
       pWitness->onEnterSpace(this);
   }
}

void Space::setSpaceData(const std::string& key, const std::string& value){
    auto iter = _spaceDatas.find(key);
    if (iter != _spaceDatas.end() && iter->second == value)
        return;

    _spaceDatas[key] = value;
	onSpaceDataChanged(key, value, false);
}

bool Space::hasSpaceData(const std::string& key){
    return _spaceDatas.find(key) != _spaceDatas.end();
}

const std::string& Space::getSpaceData(const std::string& key){
    auto iter = _spaceDatas.find(key);
    if(iter == _spaceDatas.end()){
        static const std::string null = "";
        return null;
    }

    return iter->second;
}

void Space::delSpaceData(const std::string& key){
    auto iter = _spaceDatas.find(key);
    if (iter == _spaceDatas.end())
        return;

    _spaceDatas.erase(iter);
	onSpaceDataChanged(key, "", true);
}

void Space::addSpaceDataToStream(sl::IBStream& stream){
    auto iter = _spaceDatas.begin();
    auto iterEnd = _spaceDatas.end();
    for (;iter != iterEnd; ++iter){
        stream << iter->first.c_str();
        stream << iter->second.c_str();
    }
}

IWitness* Space::setWitness(IObject* object){
    Witness* pWitness = (Witness*)object->getPropInt64(_propWitness);
    if(!pWitness){
        pWitness = NEW Witness();
        object->setPropInt64(_propWitness, (int64)pWitness);
        pWitness->attach(object);
    }
    return pWitness;
}

void Space::positionChanged(sl::api::IKernel* pKernel, IObject* object, const char* name, const IProp* prop, const bool sync){
    ObjectCoordinateNode* objectCoorNode = (ObjectCoordinateNode*)object->getPropInt64(_propCoordinateNode);
    if(!objectCoorNode){
        ERROR_LOG("positionChanged ObjectCoordinateNode is null");
        return;
    }

    objectCoorNode->update();
    printf("object[%lld] CoordinateNode updated...\n", object->getID());
}

void Space::setGeometryPath(std::string& path){
	setSpaceData("_mapping", path);
}

const std::string& Space::getGeometryPath(){
	return getSpaceData("_mapping");
}

void Space::onSpaceDataChanged(const std::string& key, const std::string& value, bool del){
}

bool Space::addSpaceGeometryMapping(const char* path, bool shouldLoadOnServer, const std::map<int32, std::string> params){
	_hasGeometry= true;
	std::string resPath = path;
	if(getGeometryPath() == path){
		ECHO_ERROR("Space::addSpaceGeometryMapping: spaceId[%d] resPath[%s] has exist", _id, path);
		return true;
	}

	setGeometryPath(resPath);

	if(shouldLoadOnServer)
		loadSpaceGeometry(params);

	return true;
}
