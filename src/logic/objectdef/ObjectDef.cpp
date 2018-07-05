#include "ObjectDef.h"
#include "slfile_utils.h"
#include "ObjectDefModule.h"
#include "DataTypeMgr.h"
#include "IDCCenter.h"
#include "NodeDefine.h"
#include "IDCCenter.h"
#include "IHarbor.h"
#include "NodeProtocol.h"
#include "IResMgr.h"
#include "pyscript/pickler.h"
bool ObjectDef::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	std::string entitiesPath = SLMODULE(ResMgr)->getPyUserScriptsPath();
	std::string aliasFilePath = entitiesPath + "defs/alias.xml";
	if (!ObjectDefModule::initialize() || !DataTypeMgr::initialize(aliasFilePath.c_str())){
		SLASSERT(false, "wtf");
		return false;
	}
	return true;
}

bool ObjectDef::launched(sl::api::IKernel * pKernel){
	if(!loadObjectsConfig(pKernel)){
		SLASSERT(false, "wtf");
		return false;
	}
    return true;
}

bool ObjectDef::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

bool ObjectDef::loadObjectsConfig(sl::api::IKernel* pKernel){
	std::string entitiesPath = SLMODULE(ResMgr)->getPyUserScriptsPath();
	std::string entitiesFile = entitiesPath + "entities.xml";
	std::string defFilePath = entitiesPath + "defs/";
	sl::XmlReader conf;
	if (!conf.loadXml(entitiesFile.c_str())){
		SLASSERT(false, "can not load file %s", entitiesFile.c_str());
		return false;
	}

	SCRIPT_TYPE_UID uType = 1;

	sl::CFileUtils::ListFileInDirection(defFilePath.c_str(), ".xml", [this](const char * name, const char * path) {
		if (_propConfigsPath.end() != _propConfigsPath.find(name)) {
			SLASSERT(false, "prop xml name repeated");
			return false;
		}
		_propConfigsPath.insert(std::make_pair(name, path));
	});

	SLMODULE(ObjectMgr)->setObjectTypeSize((int32)_propConfigsPath.size());
	const std::vector<sl::ISLXmlNode*>& allEntities = conf.root().getAllChilds();
	for (auto entity : allEntities){
		if (_modelDefMap.find(entity->value()) != _modelDefMap.end())
			continue;

		//¼ÓÔØmoduleÎÄ¼þµÄ¶¨Òå
		ObjectDefModule* pModule = loadModuleDef(pKernel, entity->value());
		if(!pModule){
			printf("load Module def[%s] is null\n", entity->value());
			return false;
		}
        _allObjectModule.push_back(pModule);
	}

	return true;
}

ObjectDefModule* ObjectDef::loadModuleDef(sl::api::IKernel* pKernel, const char* moduleName){
	string tempname = moduleName;
	auto itor = _propConfigsPath.find(moduleName);
	if (itor == _propConfigsPath.end()){
		SLASSERT(false, "wtf");
		printf("module[%s]' path is not exist\n", moduleName);
		return nullptr;
	}

	sl::XmlReader propConf;
	if (!propConf.loadXml(itor->second.c_str())){
		SLASSERT(false, "can not load file %s", itor->second.c_str());
		printf("can not load file %s\n", itor->second.c_str());
		return nullptr;
	}

	ObjectDefModule* defModule = nullptr;
	if (propConf.root().subNodeExist("Implements")){
		const sl::ISLXmlNode& implNodes = propConf.root()["Implements"];
		for (int32 i = 0; i < implNodes.count(); i++){
			if(!implNodes[i].subNodeExist("Interface"))
				continue;
			
			const sl::ISLXmlNode& interfaceNodes = implNodes[i]["Interface"];
			for (int32 j = 0; j < interfaceNodes.count(); j++){
				ObjectDefModule* implDefModule = queryModuleDef(pKernel, interfaceNodes[j].text());
				SLASSERT(implDefModule, "where is %s xml", interfaceNodes[j].text());
				if (!implDefModule){
					printf("where is implement %s xml", interfaceNodes[j].text());
					return nullptr;
				}

				if (!defModule)
					defModule = NEW ObjectDefModule(moduleName, implDefModule);
				else
					defModule->loadParentModule(implDefModule);
			}
		}
	}
	
	if (!defModule && propConf.root().subNodeExist("Parent")){
		const sl::ISLXmlNode& parentNodes = propConf.root()["Parent"];
		for (int32 i = 0; i < parentNodes.count(); i++){
			ObjectDefModule* parentDefModule = queryModuleDef(pKernel, parentNodes[i].text());
			SLASSERT(parentDefModule, "where is %s xml", parentNodes[i].text());
			if (!parentDefModule){
				printf("where is %s xml\n", parentNodes[i].text());
				return nullptr;
			}

			if (!defModule)
				defModule = NEW ObjectDefModule(moduleName, parentDefModule);
			else
				defModule->loadParentModule(parentDefModule);
		}
	}
		
	if (!defModule)
		defModule = NEW ObjectDefModule(moduleName, nullptr);

	if (!defModule->loadFrom(propConf.root())){
		printf("loadModule[%s] failed\n", moduleName);
		DEL defModule;
		return nullptr;
	}
	_modelDefMap[moduleName] = defModule;
    const int32 moduleType = SLMODULE(ObjectMgr)->getObjectType(moduleName);
    if(moduleType < 0 || _typeModelDefMap.find(moduleType) != _typeModelDefMap.end()){
        printf("invaild module type[%s] typeId[%d]\n", moduleName, moduleType);
    }else{
        _typeModelDefMap[moduleType] = defModule;
		defModule->setModuleType(moduleType);
    }
	return defModule;
}

ObjectDefModule* ObjectDef::queryModuleDef(sl::api::IKernel* pKernel, const char* moduleName){
	auto itor = _modelDefMap.find(moduleName);
	if (itor != _modelDefMap.end())
		return itor->second;

	return loadModuleDef(pKernel, moduleName);
}

bool ObjectDef::isLoadObjectModule(ObjectDefModule* defModule){
    const int32 nodeType = SLMODULE(Harbor)->getNodeType();
    switch(nodeType){
    case NodeType::LOGIC:
        {
            if(!defModule->hasBase())
                return false;
            break;
        }
    case NodeType::SCENE:
        {
            if(!defModule->hasCell())
                return false;

            break;
        }
    }

    return true;
}

const IObjectDefModule* ObjectDef::findObjectDefModule(const char* moduleName){
	auto itor = _modelDefMap.find(moduleName);
	if (itor == _modelDefMap.end()){
		return NULL;
	}
	return itor->second;
}

const IObjectDefModule* ObjectDef::findObjectDefModule(const int32 moduleType){
	auto itor = _typeModelDefMap.find(moduleType);
	if (itor == _typeModelDefMap.end()){
		return NULL;
	}
	return itor->second;
}

void ObjectDef::addAllObjectDefToStream(sl::IBStream& stream){
	const std::unordered_map<std::string, IDataType*>& allDataType = DataTypeMgr::getAllDataType();
	stream << (uint16)allDataType.size();
	for(auto dtItor : allDataType){
		IDataType* dataType = dtItor.second;
		dataType->addDataTypeInfo(stream);
	}
    const std::vector<const IObjectDefModule*>& allObjectDefs = getAllObjectDefModule();     
    for(auto objectDef : allObjectDefs){
        const char* moduleName = objectDef->getModuleName();
        stream << moduleName;
        stream << (int16)(SLMODULE(ObjectMgr)->getObjectType(moduleName));
        const PROPS_MAP& props = objectDef->getProps();
        const PROPS_MAP& clientMethods = objectDef->getClientMethods();
        const PROPS_MAP& cellMethods = objectDef->getCellMethods();
        const PROPS_MAP& baseMethods = objectDef->getBaseMethods();
        stream << (int16)props.size();
        stream << (int16)clientMethods.size();
        stream << (int16)baseMethods.size();
        stream << (int16)cellMethods.size();

        for(auto prop : props){
			printf("prop %s %p\n", prop.first.c_str(), prop.second);
            stream << prop.first.c_str();
            stream << (int16)(prop.second->getType(moduleName));           
            stream << (int32)(prop.second->getSetting(moduleName));           
			stream << (int16)-1;
			stream << "";
			IDataType* dataType = (IDataType*)(prop.second->getExtra(moduleName));
			stream << dataType->getUid();
        }

        for(auto clientMethod : clientMethods){
			printf("clientMethod %s %p\n", clientMethod.first.c_str(), clientMethod.second);
            stream << clientMethod.first.c_str();
            stream << (int16)(clientMethod.second->getIndex(moduleName));
			stream << (int16)-1;
			int32 argsSize = clientMethod.second->getSize(moduleName);
			stream << (int8)argsSize;
			IDataType** pArgsList = (IDataType**)(clientMethod.second->getExtra(moduleName));
			for(int32 i = 0; i < argsSize; i++)
				stream << pArgsList[i]->getUid();
        }
        
		for(auto baseMethod : baseMethods){
			printf("baseMethod %s %p\n", baseMethod.first.c_str(), baseMethod.second);
            stream << baseMethod.first.c_str();
            stream << (int16)baseMethod.second->getIndex(moduleName);
			stream << (int16)-1;
			int32 argsSize = baseMethod.second->getSize(moduleName);
			stream << (int8)argsSize;
			IDataType** pArgsList = (IDataType**)(baseMethod.second->getExtra(moduleName));
			for(int32 i = 0; i < argsSize; i++)
				stream << pArgsList[i]->getUid();
        }
        
		for(auto cellMethod : cellMethods){
			printf("cellMethod %s %p\n", cellMethod.first.c_str(), cellMethod.second);
            stream << cellMethod.first.c_str();
            stream << (int16)cellMethod.second->getIndex(moduleName);
			stream << (int16)-1;
			int32 argsSize = cellMethod.second->getSize(moduleName);
			stream << (int8)argsSize;
			IDataType** pArgsList = (IDataType**)(cellMethod.second->getExtra(moduleName));
			for(int32 i = 0; i < argsSize; i++)
				stream << pArgsList[i]->getUid();
        }
    } 
}


void ObjectDef::addExtraDataType(const char* typeName, IDataType* dataType){
	DataTypeMgr::addDataType(typeName, dataType);
}
