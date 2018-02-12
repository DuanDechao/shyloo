#include "EntityDef.h"
#include "slfile_utils.h"
#include "ScriptDefModule.h"
#include "DataTypeMgr.h"
#include "IDCCenter.h"
#include "NodeDefine.h"
//#include "slpymacros.h"
#include "IDCCenter.h"
#include "IHarbor.h"
#include "NodeProtocol.h"
#include "slpymacros.h"

bool EntityDef::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	
	if (!DataTypeMgr::initialize(nullptr) || !ScriptDefModule::initialize() || !loadEntitiesConfig(pKernel)){
		SLASSERT(false, "wtf");
		return false;
	}

	return true;
}

bool EntityDef::launched(sl::api::IKernel * pKernel){
	if (!loadAllScriptModules(pKernel, _scriptBaseTypes)){
		SLASSERT(false, "wtf");
		return false;
	}

    return true;
}

bool EntityDef::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

bool EntityDef::loadEntitiesConfig(sl::api::IKernel* pKernel){
	char path[256] = "/home/duandechao/shyloo/build/server/res/entities/entities.xml";
	sl::XmlReader conf;
	if (!conf.loadXml(path)){
		SLASSERT(false, "can not load file %s", path);
		return false;
	}

	SCRIPT_TYPE_UID uType = 1;

	char defPath[256] = "/home/duandechao/shyloo/build/server/res/entities/defs";
	sl::CFileUtils::ListFileInDirection(defPath, ".xml", [this](const char * name, const char * path) {
		if (_propConfigsPath.end() != _propConfigsPath.find(name)) {
			SLASSERT(false, "prop xml name repeated");
			return;
		}
		_propConfigsPath.insert(std::make_pair(name, path));
	});

	std::vector<sl::ISLXmlNode*> allEntities = conf.root().getAllChilds();
	SLMODULE(ObjectMgr)->setObjectTypeSize((int32)allEntities.size());
	for (auto entity : allEntities){
		if (_modelDefMap.find(entity->value()) != _modelDefMap.end())
			continue;

		//¼ÓÔØmoduleÎÄ¼þµÄ¶¨Òå
		ScriptDefModule* pModule = loadModuleDef(pKernel, entity->value());
        _allScriptModule.push_back(pModule);
	}

	return true;
}

ScriptDefModule* EntityDef::loadModuleDef(sl::api::IKernel* pKernel, const char* moduleName){
	auto itor = _propConfigsPath.find(moduleName);
	if (itor == _propConfigsPath.end()){
		SLASSERT(false, "wtf");
		return nullptr;
	}

	sl::XmlReader propConf;
	if (!propConf.loadXml(itor->second.c_str())){
		SLASSERT(false, "can not load file %s", itor->second.c_str());
		return nullptr;
	}

	ScriptDefModule* defModule = nullptr;
	if (propConf.root().subNodeExist("Implements")){
		const sl::ISLXmlNode& implNodes = propConf.root()["Implements"];
		for (int32 i = 0; i < implNodes.count(); i++){
			ScriptDefModule* implDefModule = queryModuleDef(pKernel, implNodes[i].value());
			SLASSERT(implDefModule, "where is %s xml", implNodes[i].value());
			if (!implDefModule)
				return nullptr;

			if (!defModule)
				defModule = NEW ScriptDefModule(moduleName, implDefModule);
			else
				defModule->loadParentModule(implDefModule);
		}
	}
	else if (propConf.root().subNodeExist("Parent")){
		const sl::ISLXmlNode& parentNodes = propConf.root()["Parent"];
		for (int32 i = 0; i < parentNodes.count(); i++){
			ScriptDefModule* parentDefModule = queryModuleDef(pKernel, parentNodes[i].value());
			SLASSERT(parentDefModule, "where is %s xml", parentNodes[i].value());
			if (!parentDefModule)
				return nullptr;

			if (!defModule)
				defModule = NEW ScriptDefModule(moduleName, parentDefModule);
			else
				defModule->loadParentModule(parentDefModule);
		}
	}
	else{
		if (!defModule)
			defModule = NEW ScriptDefModule(moduleName, nullptr);
	}

	if (!defModule->loadFrom(propConf.root())){
		DEL defModule;
		return nullptr;
	}
	_modelDefMap[moduleName] = defModule;
    const int32 modelType = SLMODULE(ObjectMgr)->getObjectType(moduleName);
    if(modelType < 0 || _typeModelDefMap.find(modelType) != _typeModelDefMap.end()){
        printf("invaild module type[%s]\n", moduleName);
    }else{
        _typeModelDefMap[modelType] = defModule;
    }
	return defModule;
}

ScriptDefModule* EntityDef::queryModuleDef(sl::api::IKernel* pKernel, const char* moduleName){
	auto itor = _modelDefMap.find(moduleName);
	if (itor != _modelDefMap.end())
		return itor->second;

	return loadModuleDef(pKernel, moduleName);
}

bool EntityDef::loadAllScriptModules(sl::api::IKernel* pKernel, std::vector<PyTypeObject*>& scriptBaseTypes){
	char path[256] = "/home/duandechao/shyloo/build/server/res/entities/entities.xml";
	sl::XmlReader conf;
	if (!conf.loadXml(path)){
		SLASSERT(false, "can not load file %s", path);
		return false;
	}

	std::vector<sl::ISLXmlNode*> allEntities = conf.root().getAllChilds();
	for (auto entity : allEntities){
		std::string moduleName = entity->value();
		ScriptDefModule* pScriptModule = (ScriptDefModule*)findScriptDefModule(moduleName.c_str());

        if(!pScriptModule){
            printf("module[%s] not found", moduleName.c_str());
        }
		
		PyObject* pyModule = PyImport_ImportModule(const_cast<char*>(moduleName.c_str()));

		//¼ì²é¸ÃÄ£¿éÂ·¾¶ÊÇ·ñÔÚ½Å±¾Ä¿Â¼ÏÂ
		if (pyModule){

		}

		if (pyModule == NULL){
           if(isLoadScriptModule(pScriptModule)){
               ERROR_LOG("could not load module[%s]", moduleName.c_str());
               PyErr_Print();
               return false;
           }

           PyErr_Clear();

           continue; 
		}

		PyObject* pyClass = PyObject_GetAttrString(pyModule, const_cast<char*>(moduleName.c_str()));
		if (pyClass == NULL){
			return false;
		}

		std::string baseTypeNames = "";
		bool vaild = false;
		for (auto baseTypes : scriptBaseTypes){
			if (!PyObject_IsSubclass(pyClass, (PyObject*)(baseTypes))){
				baseTypeNames += "'";
				baseTypeNames += baseTypes->tp_name;
				baseTypeNames += "'";
			}
			else{
				vaild = true;
				break;
			}
		}

		if (!vaild){
			SLASSERT("Class %s is not derived from shyloo[%s]", moduleName.c_str(), baseTypeNames.c_str());
			return false;
		}

		if (!PyType_Check(pyClass)){
			SLASSERT("Class %s is invaild!", moduleName.c_str());
			return false;
		}

		pScriptModule->setScriptType((PyTypeObject*)pyClass);
		S_RELEASE(pyModule);
	}
	return true;
}

bool EntityDef::isLoadScriptModule(ScriptDefModule* defModule){
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

void EntityDef::rgsBaseScriptModule(PyTypeObject* type){
	_scriptBaseTypes.push_back(type);
}

IScriptDefModule* EntityDef::findScriptDefModule(const char* moduleName){
	auto itor = _modelDefMap.find(moduleName);
	if (itor == _modelDefMap.end()){
		return NULL;
	}
	return itor->second;
}

IScriptDefModule* EntityDef::findScriptDefModule(const int32 moduleType){
	auto itor = _typeModelDefMap.find(moduleType);
	if (itor == _typeModelDefMap.end()){
		return NULL;
	}
	return itor->second;
}

