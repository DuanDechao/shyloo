#include "EntityMgr.h"
#include "IEntityDef.h"
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

EntityMgr* EntityMgr::s_self = nullptr;
bool EntityMgr::initialize(sl::api::IKernel * pKernel){
	s_self = this;
    if(SLMODULE(Harbor)->getNodeType() == NodeType::LOGIC){
         START_TIMER(s_self, 0, 1, 30000);    
    }

    if(SLMODULE(Harbor)->getNodeType() == NodeType::LOGIC){
	    SLMODULE(EntityDef)->rgsBaseScriptModule(Base::getScriptType());
	    Base::installScript("Base");
	    
        SLMODULE(EntityDef)->rgsBaseScriptModule(Proxy::getScriptType());
	    Proxy::installScript("Proxy");

	    INSTALL_SCRIPT_MODULE_METHOD(SLMODULE(PythonEngine), "createBase", &EntityMgr::__py__createBase);
	    INSTALL_SCRIPT_MODULE_METHOD(SLMODULE(PythonEngine), "createBaseLocallyFromDB", &EntityMgr::__py__createBaseLocallyFromDB);
    }

    if(SLMODULE(Harbor)->getNodeType() == NodeType::SCENE){
	    SLMODULE(EntityDef)->rgsBaseScriptModule(Entity::getScriptType());
	    Entity::installScript("Entity");
    }
	
	return true;
}

bool EntityMgr::launched(sl::api::IKernel * pKernel){
    if(SLMODULE(Harbor)->getNodeType() == NodeType::SCENE){
	    RGS_NODE_ARGS_HANDLER(SLMODULE(Harbor), NodeProtocol::REMOTE_NEW_ENTITY_MAIL, EntityMgr::onRemoteNewEntityMail);
        
        RGS_EVENT_HANDLER(SLMODULE(EventEngine), logic_event::EVENT_CREATE_CELL_ENTITY, EntityMgr::onCreateCellEntityOnCell);
    }

    if(SLMODULE(Harbor)->getNodeType() == NodeType::LOGIC){
        RGS_EVENT_HANDLER(SLMODULE(EventEngine), logic_event::EVENT_GATE_LOGINED, EntityMgr::onNewPlayerLogined);
        RGS_EVENT_HANDLER(SLMODULE(EventEngine), logic_event::EVENT_ENTITY_CREATED_FROM_DB_CALLBACK, EntityMgr::onEntityCreatedFromDB);
    }
	
    return true;
}

bool EntityMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

Proxy* EntityMgr::createProxy(const char* entityType, PyObject* params, bool initializeScript, const uint64 entityId, bool initProperty){
	Proxy* proxy = createEntity<Proxy>(entityType, params, initializeScript, entityId, initProperty);
    return proxy;
}

Base* EntityMgr::createBase(const char* entityType, PyObject* params, bool initializeScript, const uint64 entityId, bool initProperty){
	Base* base = createEntity<Base>(entityType, params, initializeScript, entityId, initProperty);
    return base;
}

Entity* EntityMgr::createCellEntity(const char* entityType, PyObject* params, bool initializeScript, const uint64 entityId, bool initProperty){
    Entity* cellEntity = createEntity<Entity>(entityType, params, initializeScript, entityId, initProperty);
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
    uint64 entityId = SLMODULE(IdMgr)->allocID();
    return SLMODULE(BaseApp)->createEntityFromDB(entityType, dbid, callbackId, entityId);
}

template<typename E>
E* EntityMgr::createEntity(const char* entityType, PyObject* params, bool isInitializeScript, const uint64 entityId, bool initProperty){
	IScriptDefModule* defModule = SLMODULE(EntityDef)->findScriptDefModule(entityType);
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

    const uint64 createId = entityId != 0 ? entityId : SLMODULE(IdMgr)->allocID(); 
    
    PyObject* obj = defModule->createPyObject(createId);
    if(!obj){
        PyErr_Format(PyExc_TypeError, "createEntity: entityType[%s] createPyObject failed\n", entityType);
        PyErr_PrintEx(0);
        return NULL;
    }

	E* entity = onCreateEntity<E>(createId, obj, defModule);
    
    if(isInitializeScript)
        entity->initializeScript();
    
    const uint64 eid = entity->getID();
    _entities[eid] = entity;

    return entity;
}

template<typename E>
E* EntityMgr::onCreateEntity(const uint64 entityId, PyObject* obj, IScriptDefModule* defModule){
	return NEW(obj) E(entityId, defModule);
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

	PyObject* e = (PyObject*)s_self->createBase(entityType, params);
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

    if(SLMODULE(EntityDef)->findScriptDefModule(entityType) == NULL){
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

void EntityMgr::onRemoteNewEntityMail(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
    int32 argsCount = args.getCount();
    
    int32 idx = 0; 
    int64 objectId = args.getInt64(idx++);
   
    auto itor = _entities.find(objectId); 
    if(itor == _entities.end()){
        printf("has no entity[%lld]\n", objectId);
        return;
    }

    Entity* entity = static_cast<Entity*>(itor->second);
    entity->onRemoteMethodCall(args, idx);
}

void EntityMgr::onCreateCellEntityOnCell(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::CreateCellEntity), "wtf");
    logic_event::CreateCellEntity* evt = (logic_event::CreateCellEntity*)context;
    
    Entity* cellEntity = createCellEntity(evt->entityType, NULL, true, evt->entityId);
    cellEntity->setBaseMailBox(evt->remoteNodeId);
}

void EntityMgr::onBaseEventCellEntityCreated(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::CellEntityCreated), "wtf");
	logic_event::CellEntityCreated* evt= (logic_event::CellEntityCreated*)context;
    
    auto itor = _entities.find(evt->entityId);
    if(itor == _entities.end()){
        ECHO_ERROR("has no base[%lld]", evt->entityId);
        return;
    }
    
    Base* base = static_cast<Base*>(itor->second);
    base->onGetCell(evt->remoteNodeId);
}

void EntityMgr::onNewPlayerLogined(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::GateLoginedInfo), "wtf");
    logic_event::GateLoginedInfo* evt = (logic_event::GateLoginedInfo*)context;

    Proxy* proxy = createProxy(evt->proxyType, NULL, true, evt->proxyId);
    proxy->setAgentId(evt->agentId);
    proxy->onEntityEnabled();
}

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
void EntityMgr::test(){
    printf("py test start++++++++++++++++++++++++++++++++++++++\n");
	PyRun_SimpleString("from ddddtest import test\ntest()\n");
}


