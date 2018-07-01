#include "EntityMgr.h"
#include "Base.h"
#include "Entity.h"
#include "Proxy.h"
#include "IPythonEngine.h"
#include "IDCCenter.h"
#include "NodeProtocol.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "IEventEngine.h"
#include "EventID.h"
#include "IMmoServer.h"
#include "IIdMgr.h"
#include "ScriptDefModule.h"
#include "EntityMailBox.h"
#include "IMonitor.h"
#include "IResMgr.h"

EntityMgr* EntityMgr::s_self = nullptr;
bool EntityMgr::initialize(sl::api::IKernel * pKernel){
	s_self = this;
	_kernel = pKernel;
    if(SLMODULE(Harbor)->getNodeType() == NodeType::LOGIC){
         START_TIMER(s_self, 0, 1, 5000);    
    }

	return true;
}

bool EntityMgr::launched(sl::api::IKernel * pKernel){
    if(SLMODULE(Harbor)->getNodeType() == NodeType::LOGIC){
		rgsBaseScriptModule(Base::getScriptType());
		Base::installScript(SLMODULE(PythonEngine)->getPythonModule());
	    
		rgsBaseScriptModule(Proxy::getScriptType());
		Proxy::installScript(SLMODULE(PythonEngine)->getPythonModule());

	    INSTALL_SCRIPT_MODULE_METHOD(SLMODULE(PythonEngine), "createBase", &EntityMgr::__py__createBase);
	    INSTALL_SCRIPT_MODULE_METHOD(SLMODULE(PythonEngine), "createBaseLocallyFromDB", &EntityMgr::__py__createBaseLocallyFromDB);
	    INSTALL_SCRIPT_MODULE_METHOD(SLMODULE(PythonEngine), "createBaseAnywhere", &EntityMgr::__py__createBaseAnywhere);
    }

    if(SLMODULE(Harbor)->getNodeType() == NodeType::SCENE){
		rgsBaseScriptModule(Entity::getScriptType());
		Entity::installScript(SLMODULE(PythonEngine)->getPythonModule());
	    
		INSTALL_SCRIPT_MODULE_METHOD(SLMODULE(PythonEngine), "addSpaceGeometryMapping", &EntityMgr::__py__addSpaceGeometryMapping);
    }

	EntityMailBox::installScript(SLMODULE(PythonEngine)->getPythonModule());

	EntityGarbages::installScript(SLMODULE(PythonEngine)->getPythonModule());
	Entities::installScript(SLMODULE(PythonEngine)->getPythonModule());

	_entities = NEW Entities();
	SLMODULE(PythonEngine)->registerPyObjectToScript("entities", _entities);

	_globalData = NEW GlobalData();
	SLMODULE(PythonEngine)->registerPyObjectToScript("globalData", _globalData);
	
	INSTALL_SCRIPT_MODULE_METHOD(SLMODULE(PythonEngine), "genUUID64", &EntityMgr::__py__genUUID64);
	INSTALL_SCRIPT_MODULE_METHOD(SLMODULE(PythonEngine), "hasRes", &EntityMgr::__py__hasRes);

	if(!loadAllScriptModules(pKernel, _scriptBaseTypes))
		return false;

	if(!loadEntryScript(pKernel))
		return false;
    
	if(SLMODULE(Harbor)->getNodeType() == NodeType::LOGIC){
        //RGS_EVENT_HANDLER(SLMODULE(EventEngine), logic_event::EVENT_ENTITY_CREATED_FROM_DB_CALLBACK, EntityMgr::onEntityCreatedFromDB);
        //RGS_EVENT_HANDLER(SLMODULE(EventEngine), logic_event::EVENT_CELL_ENTITY_CREATED, EntityMgr::onCellEntityCreatedFromCell);
        RGS_EVENT_HANDLER(SLMODULE(EventEngine), logic_event::EVENT_PROXY_CREATED, EntityMgr::onProxyCreated);
    }

	if(SLMODULE(Harbor)->getNodeType() == NodeType::SCENE){
        RGS_EVENT_HANDLER(SLMODULE(EventEngine), logic_event::EVENT_CELL_ENTITY_CREATED, EntityMgr::onCellEntityCreatedOnCell);
	}

	SLMODULE(Monitor)->addListener(this);
	
    return true;
}

bool EntityMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

bool EntityMgr::loadEntryScript(sl::api::IKernel* pKernel){
	PyObject* entryScriptName = PyUnicode_FromString("kbemain");
	_entryScript = PyImport_Import(entryScriptName);
	if(!_entryScript){
        PyErr_Print();
		return false;
	}
	return true;
}

Proxy* EntityMgr::createProxy(IObject* object, PyObject* params, bool initializeScript){
	Proxy* proxy = createEntity<Proxy>(object, params, initializeScript);
    return proxy;
}

Base* EntityMgr::createBase(IObject* object, PyObject* params, bool initializeScript){
	Base* base = createEntity<Base>(object, params, initializeScript);
    return base;
}

Entity* EntityMgr::createCellEntity(IObject* object, PyObject* params, bool initializeScript){
    Entity* cellEntity = createEntity<Entity>(object, params, initializeScript);
    return cellEntity;
}

bool EntityMgr::createBaseFromDB(const char* entityType, const uint64 dbid, PyObject* pyCallback){
    uint64 callbackId = allocCallbackId();
    if(_pyCallbacks.find(callbackId) != _pyCallbacks.end()){
        PyErr_Format(PyExc_AssertionError, "EntityMgr::createBaseLocallyFromDB: callback id has exist!");
        PyErr_PrintEx(0);
        return false;
    }

    _pyCallbacks[callbackId] = pyCallback;
    Py_INCREF(pyCallback); 
	IObject* object = CREATE_OBJECT(SLMODULE(ObjectMgr), entityType);
	onEntityCreatedFromDB(object, callbackId, false);
    //return SLMODULE(BaseApp)->createEntityFromDB(entityType, dbid, callbackId, entityId);
}

bool EntityMgr::createBaseAnywhere(const char* entityType, PyObject* params, PyObject* pyCallback){
    uint64 callbackId = allocCallbackId();
    if(_pyCallbacks.find(callbackId) != _pyCallbacks.end()){
        PyErr_Format(PyExc_AssertionError, "EntityMgr::createBaseLocallyFromDB: callback id has exist!");
        PyErr_PrintEx(0);
        return false;
    }

    _pyCallbacks[callbackId] = pyCallback;
    Py_INCREF(pyCallback); 
	IObject* object = CREATE_OBJECT(SLMODULE(ObjectMgr), entityType);
	onBaseCreateAnywhere(object, params, callbackId);
}

template<typename E>
E* EntityMgr::createEntity(IObject* object, PyObject* params, bool isInitializeScript){
	const char* entityType = object->getObjTypeString();
	ScriptDefModule* defModule = findScriptDefModule(entityType);
	int32 nodeType = SLMODULE(Harbor)->getNodeType();

    if (defModule == NULL){
		PyErr_Format(PyExc_TypeError, "createEntity: entityType[%s] not found! Please register in entities.xml and implement a %s.xml and %s.py\n", entityType, entityType, entityType);
        PyErr_PrintEx(0);
		return NULL;
	}
	else if (nodeType == NodeType::SCENE ? !defModule->hasCell() : !defModule->hasBase()){        
        PyErr_Format(PyExc_TypeError, "createEntity: cannot create %s(%s=false)! Please check the setting of the entities.xml and the implementation of %s.py\n", entityType, (nodeType == NodeType::SCENE ? "hasCell()" : "hasBase()"), entityType);
        PyErr_PrintEx(0);
        return NULL;
	}

    PyObject* obj = PyType_GenericAlloc(defModule->getScriptType(), 0);
	if (obj == NULL){
        PyErr_Format(PyExc_AssertionError, "ScriptDefModule::createPyObject: GenericAlloc is failed.\n");
		PyErr_PrintEx(0);
        return NULL;
	}
    
	SCRIPT_ERROR_CHECK();
    
	if(!obj){
        PyErr_Format(PyExc_TypeError, "createEntity: entityType[%s] createPyObject failed\n", entityType);
        PyErr_PrintEx(0);
        return NULL;
    }
	else{
		Py_INCREF(obj);
	}

	E* entity = onCreateEntity<E>(object, obj, defModule);
    
    if(isInitializeScript)
        entity->initializeEntity(params);
    
    const uint64 eid = entity->getID();
    _entities->add(eid, entity);

    return entity;
}

template<typename E>
E* EntityMgr::onCreateEntity(IObject* object, PyObject* obj, ScriptDefModule* defModule){
	return NEW(obj) E(object, defModule);
}

PyObject* EntityMgr::__py__genUUID64(PyObject* self, PyObject* args){
	return PyLong_FromUnsignedLongLong(SLMODULE(IdMgr)->allocID());
}

PyObject* EntityMgr::__py__createBase(PyObject* self, PyObject* args){
	int argCount = (int)PyTuple_Size(args);
	PyObject* params = NULL;
	char* entityType = NULL;
	int ret = -1;

	if (argCount == 2)
		ret = PyArg_ParseTuple(args, "s|O", &entityType, &params);
	else
		ret = PyArg_ParseTuple(args, "s", &entityType);

	if (entityType == NULL || ret == -1)
	{
		PyErr_Format(PyExc_AssertionError, "Baseapp::createBase: args error!");
		PyErr_PrintEx(0);
		return NULL;
	}

	IObject* object = CREATE_OBJECT(SLMODULE(ObjectMgr), entityType);
	PyObject* e = (PyObject*)s_self->createBase(object, params);
	return e;
}

PyObject* EntityMgr::__py__createBaseLocallyFromDB(PyObject* self, PyObject* args){
    int argCount = (int32)PyTuple_Size(args);
    PyObject* pyCallback = NULL;
    wchar_t* wEntityType = NULL;
    char* entityType = NULL;
    int32 ret = -1;
    uint64 dbid = 0;
    PyObject* pyEntityType = NULL;
    
    switch(argCount){
        case 3:{
            ret = PyArg_ParseTuple(args, "O|K|O", &pyEntityType, &dbid, &pyCallback);
            break;
        }
        case 2:{
            ret = PyArg_ParseTuple(args, "O|K", &pyEntityType, &dbid);
            break;
        }
        default:{
            PyErr_Format(PyExc_AssertionError, "%s: args require 2 or 3 args, gived %d!\n",
                    __FUNCTION__, argCount);
            PyErr_PrintEx(0);
            return NULL;
        }
    }

    if(ret == -1){
        PyErr_Format(PyExc_TypeError, "shyloo::createBaseLocallyFromDB: args error!");
        PyErr_PrintEx(0);
        return NULL;
    }
    
    if(pyEntityType){
        wEntityType = PyUnicode_AsWideCharString(pyEntityType, NULL);
        if(wEntityType){
            entityType = sl::CStringUtils::wchar2char(wEntityType);
            PyMem_Free(wEntityType);
        }else{
            SCRIPT_ERROR_CHECK();
        }
    }

    if(entityType == NULL || strlen(entityType) <= 0 || ret == -1){
        PyErr_Format(PyExc_AssertionError, "EntityMgr::createBaseLocallyFromDB:args error, entityType:%s",
                (entityType ? entityType : "NULL"));

        PyErr_PrintEx(0);

        if(entityType)
            free(entityType);

        return NULL;
    }

    if(SLMODULE(ObjectDef)->findObjectDefModule(entityType) == NULL){
        PyErr_Format(PyExc_AssertionError, "EntityMgr::createBaseLocallyFromDB: entityType(%s) error!", entityType);
        PyErr_PrintEx(0);
        free(entityType);
        return NULL;
    }

    if(dbid == 0){
        PyErr_Format(PyExc_AssertionError, "EntityMgr::createBaseLocallyFromDB: dbid error");
        PyErr_PrintEx(0);
        free(entityType);
        return NULL;
    }

    if(pyCallback && !PyCallable_Check(pyCallback)){
        pyCallback = NULL;
        PyErr_Format(PyExc_AssertionError, "EntityMgr::createBaseLocallyFromDB: callback error!");
        PyErr_PrintEx(0);
        free(entityType);
        return NULL;
    }

    s_self->createBaseFromDB(entityType, dbid, pyCallback);    
    
    free(entityType);

    S_Return;
}

PyObject* EntityMgr::__py__createBaseAnywhere(PyObject* self, PyObject* args){
    int argCount = (int32)PyTuple_Size(args);
	PyObject* params = NULL;
    PyObject* pyCallback = NULL;
	char* entityType = NULL;
    int32 ret = -1;
    
    switch(argCount){
        case 3:{
            ret = PyArg_ParseTuple(args, "s|O|O", &entityType, &params, &pyCallback);
            break;
        }
        case 2:{
            ret = PyArg_ParseTuple(args, "s|O", &entityType, &params);
            break;
        }
        default:{
			ret = PyArg_ParseTuple(args, "s", &entityType);
        }
    }

	if(entityType == NULL || ret == -1){
        PyErr_Format(PyExc_AssertionError, "EntityMgr::createBaseAnywhere: args error!\n");
        PyErr_PrintEx(0);
        return NULL;
	}

    if(SLMODULE(ObjectDef)->findObjectDefModule(entityType) == NULL){
        PyErr_Format(PyExc_AssertionError, "EntityMgr::createBaseAnywhere: entityType(%s) error!", entityType);
        PyErr_PrintEx(0);
        return NULL;
    }

    if(pyCallback && !PyCallable_Check(pyCallback)){
        pyCallback = NULL;
        PyErr_Format(PyExc_AssertionError, "EntityMgr::createBaseAnywhere: callback error!");
        PyErr_PrintEx(0);
        return NULL;
    }

	s_self->createBaseAnywhere(entityType, params, pyCallback);
	
	S_Return;

}

PyObject* EntityMgr::__py__hasRes(PyObject* self, PyObject* args){
	int32 argsCount = (int32)PyTuple_Size(args);
	if(argsCount != 1){
		PyErr_Format(PyExc_TypeError, "EntityMgr::hasRes(): args is error!");
		PyErr_PrintEx(0);
		return 0;
	}

	char* resPath = NULL;
	if(PyArg_ParseTuple(args, "s", &resPath) == -1){
		PyErr_Format(PyExc_TypeError, "EntityMgr::hasRes(): args is error!");
		PyErr_PrintEx(0);
		return 0;
	}

	return PyBool_FromLong(SLMODULE(ResMgr)->hasRes(resPath));

}

PyObject* EntityMgr::__py__addSpaceGeometryMapping(PyObject* self, PyObject* args){
	int32 spaceID = 0;
	PyObject* mapper = NULL;
	char* path = NULL;
	bool shouldLoadOnServer = true;
	PyObject* pyParams = NULL;
	std::map<int32, std::string> params;

	int32 argsCount = PyTuple_Size(args);
	if(argsCount < 3 || argsCount > 5){
		PyErr_Format(PyExc_AssertionError, "addSpaceGeometryMapping: args is error");
		PyErr_PrintEx(0);
		return 0;
	}

	if(argsCount == 4){
		if(PyArg_ParseTuple(args, "I|O|s|b", &spaceID, &mapper, &path, &shouldLoadOnServer) == -1){
			PyErr_Format(PyExc_AssertionError, "addSpaceGeometryMapping:: args parse error");
			PyErr_PrintEx(0);
			return 0;
		}
	}
	else if(argsCount == 5){
		if(PyArg_ParseTuple(args, "I|O|s|b|O", &spaceID, &mapper, &path, &shouldLoadOnServer, &pyParams) == -1){
			PyErr_Format(PyExc_AssertionError, "addSpaceGeometryMapping:: args parse error");
			PyErr_PrintEx(0);
			return 0;
		}
		if(pyParams){
			PyObject *key, *value;
			Py_ssize_t pos = 0;
			if(!PyDict_Check(pyParams)){
				PyErr_Format(PyExc_AssertionError, "addSpaceGeometryMapping:: args(params) is not dict");
				PyErr_PrintEx(0);
				return 0;
			}

			while(PyDict_Next(pyParams, &pos, &key, &value)){
				if(!PyLong_Check(key) || !PyUnicode_Check(value)){
					PyErr_Format(PyExc_AssertionError, "addSpaceGeometryMapping:: args(params) is error");
					PyErr_PrintEx(0);
					return 0;
				}

				uint32 i = PyLong_AsLong(key);
				wchar_t* pyUnicodeWideString = PyUnicode_AsWideCharString(value, NULL);
				char* ccattr = sl::CStringUtils::wchar2char(pyUnicodeWideString);
				PyMem_Free(pyUnicodeWideString);
				params[i] = ccattr;
				free(ccattr);
			}

			SCRIPT_ERROR_CHECK();
		}
	}
	else{
		if(PyArg_ParseTuple(args, "I|O|s", &spaceID, &mapper, &path) == -1){
			PyErr_Format(PyExc_AssertionError, "addSpaceGeometryMapping:: args(params) is error");
			PyErr_PrintEx(0);
			return 0;
		}
	}

	if(!SLMODULE(CellApp)->addSpaceGeometryMapping(spaceID, path, shouldLoadOnServer, params)){
		PyErr_Format(PyExc_AssertionError, "addSpaceGeometryMapping:: add file[%s] failed", path);
		PyErr_PrintEx(0);
		return 0;
	}

	S_Return;
}

void EntityMgr::onCellEntityCreatedOnCell(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::CellEntityCreated), "wtf");
    logic_event::CellEntityCreated* evt = (logic_event::CellEntityCreated*)context;
    Entity* cellEntity = createCellEntity(evt->object, NULL, false);
    cellEntity->setBaseMailBox(evt->baseNodeId);
    
    bool ret = cellEntity->createCellDataFromStream(evt->cellData, evt->cellDataSize);
    if(!ret){
        SLASSERT(false, "createCellData Failed");
        return;
    } 

    cellEntity->initializeEntity(NULL);
}

void EntityMgr::onCellEntityCreatedFromCell(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::CellEntityCreatedFromCell), "wtf");
	logic_event::CellEntityCreatedFromCell* evt= (logic_event::CellEntityCreatedFromCell*)context;
    
    EntityScriptObject* obj = _entities->find(evt->entityId);
    if(!obj){
        ECHO_ERROR("has no base[%lld]", evt->entityId);
        return;
    }
    
    Base* base = static_cast<Base*>(obj);
    base->onGetCell(evt->remoteNodeId);
}

void EntityMgr::onProxyCreated(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::CellEntityCreated), "wtf");
	logic_event::ProxyCreated* evt = (logic_event::ProxyCreated*)context;
	Proxy* proxy = createProxy(evt->object, NULL, true); 
    proxy->onEntityEnabled(evt->agentId);
}

void EntityMgr::onEntityCreatedFromDB(IObject* object, const int32 callbackId, bool wasActive){
    PyObject* e = createBase(object, NULL, true);
    if(callbackId > 0){
        auto callbackItor = _pyCallbacks.find(callbackId);
        if(callbackItor == _pyCallbacks.end())
            return;
        
        PyObject* pyfunc = callbackItor->second;
        PyObject* pyResult = PyObject_CallFunction(pyfunc, const_cast<char*>("OKi"), e, object->getID(), wasActive);
        if(pyResult != NULL)
            Py_DECREF(pyResult);
        else
            SCRIPT_ERROR_CHECK();
        
        Py_DECREF(pyfunc);
        _pyCallbacks.erase(callbackItor);
    }
}

void EntityMgr::onBaseCreateAnywhere(IObject* object,  PyObject* params, const int32 callbackId){
	PyObject* e = createBase(object, params, true);
    if(callbackId > 0){
        auto callbackItor = _pyCallbacks.find(callbackId);
        if(callbackItor == _pyCallbacks.end())
            return;

		PyObject* pyArgs = PyTuple_New(1);
        PyObject* pyfunc = callbackItor->second;
		
		PyObject* base = _entities->find(object->getID());
		if(!base){
			ECHO_ERROR("EntityMgr::onBaseCreateFromAnywhere:: can't found entity[%lld]", object->getID());
			Py_DECREF(pyArgs);
			return;
		}
		
		Py_INCREF(base);
		PyTuple_SET_ITEM(pyArgs, 0, base);
        PyObject* pyResult = PyObject_CallObject(pyfunc, pyArgs);
        if(pyResult != NULL)
            Py_DECREF(pyResult);
        else
            SCRIPT_ERROR_CHECK();
        
        Py_DECREF(pyfunc);
		Py_DECREF(pyArgs);
        _pyCallbacks.erase(callbackItor);
    }
}
/*
void EntityMgr::onEntityCreatedFromDB(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::EntityCreatedFromDBCallBack), "wtf");
    logic_event::EntityCreatedFromDBCallBack* evt = (logic_event::EntityCreatedFromDBCallBack*)context;
    
    PyObject* e = createBase(evt->entityType, NULL, true, evt->entityId);
    if(evt->callbackId > 0){
        auto callbackItor = _pyCallbacks.find(evt->callbackId);
        if(callbackItor == _pyCallbacks.end())
            return;
        
        PyObject* pyfunc = callbackItor->second;
        PyObject* pyResult = PyObject_CallFunction(pyfunc, const_cast<char*>("OKi"), e, evt->dbid, evt->wasActive);
        if(pyResult != NULL)
            Py_DECREF(pyResult);
        else
            SCRIPT_ERROR_CHECK();
        
        Py_DECREF(pyfunc);
        _pyCallbacks.erase(callbackItor);
    }
}
*/
void EntityMgr::rgsBaseScriptModule(PyTypeObject* type){
	_scriptBaseTypes.push_back(type);
}

bool EntityMgr::loadAllScriptModules(sl::api::IKernel* pKernel, std::vector<PyTypeObject*>& scriptBaseTypes){	
	const std::vector<const IObjectDefModule*>& allObjectDefModule = SLMODULE(ObjectDef)->getAllObjectDefModule();
	for (auto defModule : allObjectDefModule){
		const char* moduleName = defModule->getModuleName();
		PyObject* pyModule = PyImport_ImportModule(const_cast<char*>(moduleName));

		//¼ì²é¸ÃÄ£¿éÂ·¾¶ÊÇ·ñÔÚ½Å±¾Ä¿Â¼ÏÂ
		if (pyModule){

		}

		if (pyModule == NULL){
           if(isLoadScriptModule(defModule)){
               ERROR_LOG("could not load module[%s]", moduleName);
               PyErr_Print();
               return false;
           }

           PyErr_Clear();

           continue; 
		}

		PyObject* pyClass = PyObject_GetAttrString(pyModule, const_cast<char*>(moduleName));
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
			SLASSERT("Class %s is not derived from shyloo[%s]", moduleName, baseTypeNames.c_str());
			return false;
		}

		if (!PyType_Check(pyClass)){
			SLASSERT("Class %s is invaild!", moduleName);
			return false;
		}
		_scriptDefModules[moduleName] = NEW ScriptDefModule(defModule);
		_scriptDefModules[moduleName]->setScriptType((PyTypeObject*)pyClass);
		_typeToScriptDefModules[defModule->getModuleType()] = _scriptDefModules[moduleName];
		S_RELEASE(pyModule);
	}
	return true;
}

bool EntityMgr::isLoadScriptModule(const IObjectDefModule* defModule){
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


ScriptDefModule* EntityMgr::findScriptDefModule(const char* entityType){
	auto itor = _scriptDefModules.find(entityType);
	if(itor != _scriptDefModules.end())
		return itor->second;
	return nullptr;
}

ScriptDefModule* EntityMgr::findScriptDefModule(const int32 entityType){
	auto itor = _typeToScriptDefModules.find(entityType);
	if(itor != _typeToScriptDefModules.end())
		return itor->second;
	return nullptr;
}


void EntityMgr::onRemoteMethodCall(const uint64 objectId, const sl::OBStream& stream){
	EntityScriptObject* obj = _entities->find(objectId);	
	if(!obj){
		ECHO_ERROR("cant find object[%lld]", objectId);
		return;
	}

	obj->onRemoteMethodCall(stream);
}

void EntityMgr::test(){
    printf("py test start++++++++++++++++++++++++++++++++++++++\n");
	PyRun_SimpleString("from ddddtest import test\ntest()\n");
}

bool EntityMgr::onServerReady(sl::api::IKernel* pKernel){
	PyObject* pyResult = PyObject_CallMethod(_entryScript, const_cast<char*>("onBaseAppReady"), const_cast<char*>("O"), PyBool_FromLong(1));
	if(pyResult != NULL)
		Py_DECREF(pyResult);
	else
		SCRIPT_ERROR_CHECK();
	
	return true;
}

bool EntityMgr::onServerReadyForLogin(sl::api::IKernel* pKernel){
	if(PyObject_HasAttrString(_entryScript, "onReadyForLogin") > 0){
		PyObject* pyResult = PyObject_CallMethod(_entryScript, const_cast<char*>("onReadyForLogin"), const_cast<char*>("O"), PyBool_FromLong(1));
		if(pyResult != NULL){
			bool completed = (pyResult == Py_True);
			Py_DECREF(pyResult);
			return completed;
		}
		else{
			SCRIPT_ERROR_CHECK();
			return false;
		}
	}
}

bool EntityMgr::onServerReadyForShutDown(sl::api::IKernel* pKernel){
	return true;
}

bool EntityMgr::onServerShutDown(sl::api::IKernel* pKernel){
	return true;
}
