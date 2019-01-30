#include "PythonServer.h"
#include "Base.h"
#include "Entity.h"
#include "Proxy.h"
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
//#include "IResMgr.h"
#include "PyDataType.h"
#include "pyscript/py_gc.h"
#include "IPythonEngine.h"
#include "ITelnetServer.h"
#include "IDebugHelper.h"

PythonServer* PythonServer::s_self = nullptr;
bool PythonServer::initialize(sl::api::IKernel * pKernel){
	s_self = this;
	_kernel = pKernel;
	return PyDataType::initialize();
}

bool PythonServer::launched(sl::api::IKernel * pKernel){
	if(SLMODULE(Harbor)->getNodeType() != NodeType::LOGIC && SLMODULE(Harbor)->getNodeType() != NodeType::SCENE)
		return true;
	const std::vector<const IObjectDefModule*>& allObjectDefs = SLMODULE(ObjectDef)->getAllObjectDefModule();     
	for(auto defModule : allObjectDefs){
		const char* moduleName = defModule->getModuleName();
		_propPyObject = SLMODULE(ObjectMgr)->appendObjectProp(moduleName, "pyObject", DTYPE_INT64, sizeof(int64), 0, 0, 0);
	}
    
	if(SLMODULE(Harbor)->getNodeType() == NodeType::LOGIC){
		rgsBaseScriptModule(Base::getScriptType());
		Base::installScript(SLMODULE(PythonEngine)->getBaseModule());
	    
		rgsBaseScriptModule(Proxy::getScriptType());
		Proxy::installScript(SLMODULE(PythonEngine)->getBaseModule());

	    SLMODULE(PythonEngine)->installScriptModuleMethod("createBase", &PythonServer::__py__createBase);
	    SLMODULE(PythonEngine)->installScriptModuleMethod("createBaseLocally", &PythonServer::__py__createBase);
	    SLMODULE(PythonEngine)->installScriptModuleMethod("createBaseLocallyFromDB", &PythonServer::__py__createBaseLocallyFromDB);
	    SLMODULE(PythonEngine)->installScriptModuleMethod("createBaseAnywhere", &PythonServer::__py__createBaseAnywhere);
    }

    if(SLMODULE(Harbor)->getNodeType() == NodeType::SCENE){
		rgsBaseScriptModule(Entity::getScriptType());
		Entity::installScript(SLMODULE(PythonEngine)->getBaseModule());
	    
		SLMODULE(PythonEngine)->installScriptModuleMethod("addSpaceGeometryMapping", &PythonServer::__py__addSpaceGeometryMapping);
	    SLMODULE(PythonEngine)->installScriptModuleMethod("createEntity", &PythonServer::__py__createEntity);
    }

	SLMODULE(PythonEngine)->installScriptModuleMethod("scriptLogType", &PythonServer::__py__scriptLogType);
	SLMODULE(PythonEngine)->installScriptModuleMethod("publish", &PythonServer::__py__publish);
	SLMODULE(PythonEngine)->installScriptModuleMethod("addWatcher", &PythonServer::__py__addWatcher);
	

	EntityMailBox::installScript(SLMODULE(PythonEngine)->getBaseModule());

	EntityGarbages::installScript(SLMODULE(PythonEngine)->getBaseModule());
	Entities::installScript(SLMODULE(PythonEngine)->getBaseModule());

	_entities = NEW Entities();
	SLMODULE(PythonEngine)->registerPyObjectToScript("entities", _entities);

	_globalData = NEW GlobalData();
	SLMODULE(PythonEngine)->registerPyObjectToScript("globalData", _globalData);
	
	SLMODULE(PythonEngine)->installScriptModuleMethod("genUUID64", &PythonServer::__py__genUUID64);
	SLMODULE(PythonEngine)->installScriptModuleMethod("hasRes", &PythonServer::__py__hasRes);
	SLMODULE(PythonEngine)->installScriptModuleMethod("getResFullPath", &PythonServer::__py__getResFullPath);

	SLMODULE(PythonEngine)->addScriptIntConstant("LOG_TYPE_NORMAL", 0);
	SLMODULE(PythonEngine)->addScriptIntConstant("LOG_TYPE_DBG", 1);
	SLMODULE(PythonEngine)->addScriptIntConstant("LOG_TYPE_INFO", 2);
	SLMODULE(PythonEngine)->addScriptIntConstant("LOG_TYPE_WAR", 3);
	SLMODULE(PythonEngine)->addScriptIntConstant("LOG_TYPE_ERR", 4);

	if(!loadAllScriptModules(pKernel, _scriptBaseTypes))
		return false;

	if(!loadEntryScript(pKernel))
		return false;
    
	if(SLMODULE(Harbor)->getNodeType() == NodeType::LOGIC){
        RGS_EVENT_HANDLER(SLMODULE(EventEngine), logic_event::EVENT_ENTITY_CREATED_FROM_DB_CALLBACK, PythonServer::onBaseCreatedFromDB);
        RGS_EVENT_HANDLER(SLMODULE(EventEngine), logic_event::EVENT_CELL_ENTITY_CREATED, PythonServer::onCellEntityCreatedFromCell);
        RGS_EVENT_HANDLER(SLMODULE(EventEngine), logic_event::EVENT_PROXY_CREATED, PythonServer::onProxyCreated);
        RGS_EVENT_HANDLER(SLMODULE(EventEngine), logic_event::EVENT_LOGIC_CREATE_BASE_ANYWHERE, PythonServer::onCreateBaseAnywhere);
        RGS_EVENT_HANDLER(SLMODULE(EventEngine), logic_event::EVENT_LOGIC_CREATE_BASE_ANYWHERE_CALLBACK, PythonServer::onCreateBaseAnywhereCallback);
    }

	if(SLMODULE(Harbor)->getNodeType() == NodeType::SCENE){
        RGS_EVENT_HANDLER(SLMODULE(EventEngine), logic_event::EVENT_CELL_ENTITY_CREATED, PythonServer::onCellEntityCreatedOnCell);
	}

    RGS_EVENT_HANDLER(SLMODULE(EventEngine), logic_event::EVENT_REMOTE_METHOD_CALL, PythonServer::onRemoteMethodCall);
	RGS_NODE_HANDLER(SLMODULE(Harbor), NodeProtocol::REMOTE_NEW_ENTITY_MAIL, PythonServer::onRemoteNewEntityMail);

	SLMODULE(Cluster)->addServerProcessHandler(this);
	SLMODULE(TelnetServer)->rgsTelnetHandler("python", this);
	
    return true;
}

bool PythonServer::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

IDataType* PythonServer::createPyDataTypeFromXml(const char* typeName, const sl::ISLXmlNode* type){
	return PyDataType::createDataType(typeName, type);
} 
IDataType* PythonServer::createPyDataType(const char* typeName){
	return PyDataType::createDataType(typeName);
}

bool PythonServer::loadEntryScript(sl::api::IKernel* pKernel){
	PyObject* entryScriptName = PyUnicode_FromString("kbemain");
	_entryScript = PyImport_Import(entryScriptName);
	if(!_entryScript){
        PyErr_Print();
		return false;
	}
	return true;
}

Proxy* PythonServer::createProxy(IObject* object, PyObject* params, bool initializeScript){
	Proxy* proxy = createEntity<Proxy>(object, params, initializeScript);
    return proxy;
}

Base* PythonServer::createBase(IObject* object, PyObject* params, bool initializeScript){
	Base* base = createEntity<Base>(object, params, initializeScript);
    return base;
}

Entity* PythonServer::createCellEntity(IObject* object, PyObject* params, bool initializeScript){
    Entity* cellEntity = createEntity<Entity>(object, params, initializeScript);
    return cellEntity;
}

bool PythonServer::createBaseFromDB(const char* entityType, const uint64 dbid, PyObject* pyCallback){
    uint64 callbackId = allocCallbackId();
    if(_pyCallbacks.find(callbackId) != _pyCallbacks.end()){
        PyErr_Format(PyExc_AssertionError, "PythonServer::createBaseLocallyFromDB: callback id has exist!");
        PyErr_PrintEx(0);
        return false;
    }

    _pyCallbacks[callbackId] = pyCallback;
    Py_INCREF(pyCallback); 
    return SLMODULE(BaseApp)->createBaseFromDB(entityType, dbid, callbackId);
}

bool PythonServer::createBaseAnywhere(const char* entityType, PyObject* params, PyObject* pyCallback){
    uint64 callbackId = pyCallback ? allocCallbackId() : 0;
    if(callbackId > 0 && _pyCallbacks.find(callbackId) != _pyCallbacks.end()){
        PyErr_Format(PyExc_AssertionError, "PythonServer::createBaseLocallyFromDB: callback id has exist!");
        PyErr_PrintEx(0);
        return false;
    }
    
	if(callbackId > 0){
		_pyCallbacks[callbackId] = pyCallback;
		Py_INCREF(pyCallback); 
	}

	std::string strInitData = "";
	if(params != NULL && PyDict_Check(params)){
		strInitData = SLMODULE(PythonEngine)->pickle(params);
	}

	SLMODULE(BaseApp)->createBaseAnywhere(entityType, strInitData.data(), strInitData.size(), callbackId);
}

template<typename E>
E* PythonServer::createEntity(IObject* object, PyObject* params, bool isInitializeScript){
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
E* PythonServer::onCreateEntity(IObject* object, PyObject* obj, ScriptDefModule* defModule){
	return NEW(obj) E(object, defModule);
}

PyObject* PythonServer::__py__genUUID64(PyObject* self, PyObject* args){
	return PyLong_FromUnsignedLongLong(SLMODULE(IdMgr)->allocID());
}

PyObject* PythonServer::__py__scriptLogType(PyObject* self, PyObject* args){
	S_Return;
}

PyObject* PythonServer::__py__publish(PyObject* self, PyObject* args){
	return PyLong_FromUnsignedLong(0);
}

PyObject* PythonServer::__py__addWatcher(PyObject* self, PyObject* args){
	S_Return;
}

PyObject* PythonServer::__py__createBase(PyObject* self, PyObject* args){
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

PyObject* PythonServer::__py__createBaseLocallyFromDB(PyObject* self, PyObject* args){
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
        PyErr_Format(PyExc_AssertionError, "PythonServer::createBaseLocallyFromDB:args error, entityType:%s",
                (entityType ? entityType : "NULL"));

        PyErr_PrintEx(0);

        if(entityType)
            free(entityType);

        return NULL;
    }

    if(SLMODULE(ObjectDef)->findObjectDefModule(entityType) == NULL){
        PyErr_Format(PyExc_AssertionError, "PythonServer::createBaseLocallyFromDB: entityType(%s) error!", entityType);
        PyErr_PrintEx(0);
        free(entityType);
        return NULL;
    }

    if(dbid == 0){
        PyErr_Format(PyExc_AssertionError, "PythonServer::createBaseLocallyFromDB: dbid error");
        PyErr_PrintEx(0);
        free(entityType);
        return NULL;
    }

    if(pyCallback && !PyCallable_Check(pyCallback)){
        pyCallback = NULL;
        PyErr_Format(PyExc_AssertionError, "PythonServer::createBaseLocallyFromDB: callback error!");
        PyErr_PrintEx(0);
        free(entityType);
        return NULL;
    }

    s_self->createBaseFromDB(entityType, dbid, pyCallback);    
    
    free(entityType);

    S_Return;
}

PyObject* PythonServer::__py__createBaseAnywhere(PyObject* self, PyObject* args){
	printf("dddddsssssssssssssssssssssssssssss\n");
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
        PyErr_Format(PyExc_AssertionError, "PythonServer::createBaseAnywhere: args error!\n");
        PyErr_PrintEx(0);
        return NULL;
	}

    if(SLMODULE(ObjectDef)->findObjectDefModule(entityType) == NULL){
        PyErr_Format(PyExc_AssertionError, "PythonServer::createBaseAnywhere: entityType(%s) error!", entityType);
        PyErr_PrintEx(0);
        return NULL;
    }

    if(pyCallback && !PyCallable_Check(pyCallback)){
        pyCallback = NULL;
        PyErr_Format(PyExc_AssertionError, "PythonServer::createBaseAnywhere: callback error!");
        PyErr_PrintEx(0);
        return NULL;
    }

	s_self->createBaseAnywhere(entityType, params, pyCallback);
	
	S_Return;

}

PyObject* PythonServer::__py__hasRes(PyObject* self, PyObject* args){
	int32 argsCount = (int32)PyTuple_Size(args);
	if(argsCount != 1){
		PyErr_Format(PyExc_TypeError, "PythonServer::hasRes(): args is error!");
		PyErr_PrintEx(0);
		return 0;
	}

	char* resPath = NULL;
	if(PyArg_ParseTuple(args, "s", &resPath) == -1){
		PyErr_Format(PyExc_TypeError, "PythonServer::hasRes(): args is error!");
		PyErr_PrintEx(0);
		return 0;
	}

	return PyBool_FromLong(s_self->getKernel()->hasRes(resPath));

}

PyObject* PythonServer::__py__getResFullPath(PyObject* self, PyObject* args){
	int32 argsCount = (int32)PyTuple_Size(args);
	if(argsCount != 1){
		PyErr_Format(PyExc_TypeError, "PythonServer::hasRes(): args is error!");
		PyErr_PrintEx(0);
		return 0;
	}

	char* resPath = NULL;
	if(PyArg_ParseTuple(args, "s", &resPath) == -1){
		PyErr_Format(PyExc_TypeError, "PythonServer::hasRes(): args is error!");
		PyErr_PrintEx(0);
		return 0;
	}

	const char* fullResPath = s_self->getKernel()->matchRes(resPath);
	return PyUnicode_FromString(fullResPath); 
}

PyObject* PythonServer::__py__addSpaceGeometryMapping(PyObject* self, PyObject* args){
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
	
PyObject* PythonServer::__py__createEntity(PyObject* self, PyObject* args){
	PyObject* params = NULL;
	char* entityType = NULL;
	int32 spaceId = 0;
	PyObject* position, *direction;
	
	if(!PyArg_ParseTuple(args, "s|I|O|O|O", &entityType, &spaceId, &position, &direction, &params)){
		PyErr_Format(PyExc_TypeError, "PythonServer::createEntity: args is error");
		PyErr_PrintEx(0);
		return 0;
	}

	if(entityType == NULL || strlen(entityType)  == 0){
		PyErr_Format(PyExc_TypeError, "PythonServer::createEntity: entityType is null");
		PyErr_PrintEx(0);
		return 0;
	}

	Position3D pos, dir;
	sl::pyscript::ScriptVector3::convertPyObjectToVector3(pos, position);
	sl::pyscript::ScriptVector3::convertPyObjectToVector3(dir, direction);
	
	IObject* object = SLMODULE(CellApp)->createEntity(entityType, spaceId, pos, dir, params, 0);
	if(!object){
		SLASSERT(false, "create Entity failed");
		return NULL;
	}
	return (PyObject*)(object->getPropInt64(s_self->getPropPyObject()));
}

void PythonServer::onRemoteMethodCall(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::RemoteMethodCall), "wtf");
    logic_event::RemoteMethodCall* evt = (logic_event::RemoteMethodCall*)context;
	
	EntityScriptObject* obj = _entities->find(evt->objectId);	
	if(!obj){
		ECHO_ERROR("cant find object[%lld]", evt->objectId);
		return;
	}

	obj->onRemoteMethodCall(evt->stream);
}

void PythonServer::onRemoteNewEntityMail(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const sl::OBStream& args){
	int8 mType = 0;
	uint64 entityId = 0;
	args >> mType >> entityId;
	
	EntityScriptObject* obj = _entities->find(entityId);	
	if(!obj){
		ECHO_ERROR("cant find object[%lld]", entityId);
		return;
	}
	
	EntityMailBox* mailBox = NULL;
	if(SLMODULE(Harbor)->getNodeType() == NodeType::LOGIC){
		if(mType == EntityMailBoxType::MAILBOX_TYPE_BASE)
			obj->onRemoteMethodCall(args);
		else if(mType == EntityMailBoxType::MAILBOX_TYPE_CELL_VIA_BASE){
			mailBox = static_cast<Base*>(obj)->getCellMailBox();
		}
		else if(mType == EntityMailBoxType::MAILBOX_TYPE_CLIENT_VIA_BASE)
			mailBox = static_cast<Base*>(obj)->getClientMailBox();
		else{
			SLASSERT(false, "invaild mailbox call");
		}
		
	}
	else if(SLMODULE(Harbor)->getNodeType() == NodeType::SCENE){
		if(mType == EntityMailBoxType::MAILBOX_TYPE_CELL)
			obj->onRemoteMethodCall(args);
		else if(mType == EntityMailBoxType::MAILBOX_TYPE_BASE_VIA_CELL)
			mailBox = static_cast<Entity*>(obj)->getBaseMailBox();
		else if(mType == EntityMailBoxType::MAILBOX_TYPE_CLIENT_VIA_CELL)
			mailBox = static_cast<Entity*>(obj)->getClientMailBox();
		else{
			SLASSERT(false, "invaild mailBox call");
		}
	}

	if(mailBox){
		sl::BStream<1000> stream;
		mailBox->newMail(stream);
		stream << args;
		mailBox->postMail(stream.out());
	}
}

void PythonServer::onCellEntityCreatedOnCell(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::CellEntityCreated), "wtf");
    logic_event::CellEntityCreated* evt = (logic_event::CellEntityCreated*)context;
    Entity* cellEntity = createCellEntity(evt->object, NULL, false);
	if(evt->baseNodeId > 0)
		cellEntity->setBaseMailBox(evt->baseNodeId);

	if(evt->hasClient && evt->baseNodeId > 0)
		cellEntity->setClientMailBox(evt->baseNodeId);
    
	if(evt->cellData && evt->cellDataSize > 0){
		bool ret = cellEntity->createCellDataFromStream(evt->cellData, evt->cellDataSize);
		if(!ret){
			SLASSERT(false, "createCellData Failed");
			return;
		} 
	}

    cellEntity->initializeEntity((PyObject*)evt->initData);
	evt->object->setPropInt64(_propPyObject, (int64)cellEntity);
}

void PythonServer::onCellEntityCreatedFromCell(sl::api::IKernel* pKernel, const void* context, const int32 size){
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

void PythonServer::onProxyCreated(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::ProxyCreated), "wtf");
	logic_event::ProxyCreated* evt = (logic_event::ProxyCreated*)context;
	Proxy* proxy = createProxy(evt->object, NULL, true); 
    proxy->onEntityEnabled(evt->agentId);
}
    
void PythonServer::onCreateBaseAnywhere(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::CreateBaseAnywhere), "wtf");
	logic_event::CreateBaseAnywhere* evt = (logic_event::CreateBaseAnywhere*)context;

	PyObject* params = NULL;
	if(evt->dataSize > 0){
		std::string strInitData((const char*)evt->initData, evt->dataSize);
		params = SLMODULE(PythonEngine)->unpickle(strInitData);
	}
	PyObject* e = createBase(evt->object, params, true);
}

void PythonServer::onCreateBaseAnywhereCallback(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::CreateBaseAnywhereCallback), "wtf");
	logic_event::CreateBaseAnywhereCallback* evt = (logic_event::CreateBaseAnywhereCallback*)context;
	if(evt->callbackId == 0)
		return;

    auto callbackItor = _pyCallbacks.find(evt->callbackId);
    if(callbackItor == _pyCallbacks.end()){
		SLASSERT(false, "has no callback[%lld]", evt->callbackId);
		return;
	}

	PyObject* pyArgs = PyTuple_New(1);
    PyObject* pyfunc = callbackItor->second;
	PyObject* pyObj = NULL;
	if(evt->createNodeId == SLMODULE(Harbor)->getNodeId()){
		pyObj = _entities->find(evt->entityId);
		if(!pyObj){
			ECHO_ERROR("PythonServer::onBaseCreateFromAnywhere:: can't found entity[%lld]", evt->entityId);
			Py_DECREF(pyArgs);
			return;
		}
		Py_INCREF(pyObj);
	}
	else{
		ScriptDefModule* pScriptDefModule = findScriptDefModule(evt->entityType);
		if(!pScriptDefModule){
			ECHO_ERROR("PythonServer::onCreateBaseAnywhereCallback: cant found entityType[%s]", evt->entityType);
			Py_DECREF(pyArgs);
			return;
		}
		pyObj = NEW EntityMailBox(EntityMailBoxType::MAILBOX_TYPE_BASE, evt->createNodeId, evt->entityId, pScriptDefModule);
	}

	PyTuple_SET_ITEM(pyArgs, 0, pyObj);
    PyObject* pyResult = PyObject_CallObject(pyfunc, pyArgs);
    if(pyResult != NULL)
        Py_DECREF(pyResult);
    else
        SCRIPT_ERROR_CHECK();
    
    Py_DECREF(pyfunc);
	Py_DECREF(pyArgs);
    _pyCallbacks.erase(callbackItor);
}

void PythonServer::onBaseCreatedFromDB(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::EntityCreatedFromDBCallBack), "wtf");
    logic_event::EntityCreatedFromDBCallBack* evt = (logic_event::EntityCreatedFromDBCallBack*)context;
    
    PyObject* e = createBase(evt->object, NULL, false);
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
	static_cast<EntityScriptObject*>(e)->initializeEntity(NULL);
}

void PythonServer::rgsBaseScriptModule(PyTypeObject* type){
	_scriptBaseTypes.push_back(type);
}

bool PythonServer::loadAllScriptModules(sl::api::IKernel* pKernel, std::vector<PyTypeObject*>& scriptBaseTypes){	
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

bool PythonServer::isLoadScriptModule(const IObjectDefModule* defModule){
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


ScriptDefModule* PythonServer::findScriptDefModule(const char* entityType){
	auto itor = _scriptDefModules.find(entityType);
	if(itor != _scriptDefModules.end())
		return itor->second;
	return nullptr;
}

ScriptDefModule* PythonServer::findScriptDefModule(const int32 entityType){
	auto itor = _typeToScriptDefModules.find(entityType);
	if(itor != _typeToScriptDefModules.end())
		return itor->second;
	return nullptr;
}

void PythonServer::test(){
//    printf("py test start++++++++++++++++++++++++++++++++++++++\n");
//	PyRun_SimpleString("from ddddtest import test\ntest()\n");

	uint64 val = 2220061311037865984;
	PyObject* pyVal = PyLong_FromUnsignedLongLong(val);
	if(PyErr_Occurred()){
		SLASSERT(false, "wtf");
		PyErr_Format(PyExc_TypeError, "UInt64Type::createFromStream: errval=%lld", val);
		PyErr_PrintEx(0);
		//SL_RELEASE(pyVal);
		if(pyVal){
			Py_DECREF(pyVal);
			pyVal = NULL;
		}
	//	return PyLong_FromUnsignedLongLong(0);
	}
}

bool PythonServer::onServerReady(sl::api::IKernel* pKernel){
	if(PyObject_HasAttrString(_entryScript, "onBaseAppReady") <= 0)
		return true;

	PyObject* pyResult = PyObject_CallMethod(_entryScript, const_cast<char*>("onBaseAppReady"), const_cast<char*>("O"), PyBool_FromLong(1));
	if(pyResult != NULL)
		Py_DECREF(pyResult);
	else
		SCRIPT_ERROR_CHECK();
	
	return true;
}

bool PythonServer::onServerReadyForLogin(sl::api::IKernel* pKernel){
	bool completed = false;
	float v = 0.0f;
	if(PyObject_HasAttrString(_entryScript, "onReadyForLogin") > 0){
		PyObject* pyResult = PyObject_CallMethod(_entryScript, const_cast<char*>("onReadyForLogin"), const_cast<char*>("O"), PyBool_FromLong(1));
		if(pyResult != NULL){
			completed = (pyResult == Py_True);
			if(!completed){
				v = (float)PyFloat_AsDouble(pyResult);
				if(PyErr_Occurred()){
					SCRIPT_ERROR_CHECK();
					Py_DECREF(pyResult);
					return false;
				}
			}
			else{
				v = 100.f;
			}
			Py_DECREF(pyResult);
		}
		else{
			SCRIPT_ERROR_CHECK();
			return false;
		}
	}
	else{
		v = 100.f;
		return true;
	}

	if(v > 0.9999f){
		v = 100.f;
		completed = true;
	}

	if(!completed){
		ECHO_TRACE("complete progress[%f]", v);
	}
	return completed;
}

bool PythonServer::onServerReadyForShutDown(sl::api::IKernel* pKernel){
	return true;
}

bool PythonServer::onServerShutDown(sl::api::IKernel* pKernel){
	return true;
}

bool PythonServer::processTelnetCommand(std::string command, std::string& retBuf){
	return SLMODULE(PythonEngine)->runSimpleString(command, retBuf);
}
