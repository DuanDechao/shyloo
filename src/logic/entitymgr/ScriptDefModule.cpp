#include "ScriptDefModule.h"
#include "slxml_reader.h"
#include "slfile_utils.h"
#include "IDCCenter.h"
#include "DataTypeMgr.h"
#include "NodeDefine.h"
#include "slstring_utils.h"
#include "NodeProtocol.h"
#include "GameDefine.h"
#include "IHarbor.h"
#include "Attr.h"
#include "EntityScriptObject.h"

ScriptDefModule::ScriptDefModule(const IObjectDefModule* objectDefModule)
	:_objectDefModule(objectDefModule),
	_propDict(PyDict_New()),
    _cellPropData(PyDict_New()),
    _cellMethodDict(PyDict_New()),
    _baseMethodDict(PyDict_New()),
    _clientMethodDict(PyDict_New()),
	_idToProps(_objectDefModule->getIdToProps()),
	_idToClientMethods(_objectDefModule->getIdToClientMethods()),
	_idToBaseMethods(_objectDefModule->getIdToBaseMethods()),
	_idToCellMethods(_objectDefModule->getIdToCellMethods()),
    _remoteEntityMethod(nullptr)
{
	initialize();
}

ScriptDefModule::~ScriptDefModule(){
	Py_DECREF(_propDict);
    Py_DECREF(_cellMethodDict);
    Py_DECREF(_baseMethodDict);
    Py_DECREF(_clientMethodDict);
}

bool ScriptDefModule::initialize(){
	const PROPS_MAP& props = _objectDefModule->getProps();
	const PROPS_MAP& clientMethods = _objectDefModule->getClientMethods();
	const PROPS_MAP& cellMethods = _objectDefModule->getCellMethods();
	const PROPS_MAP& baseMethods = _objectDefModule->getBaseMethods();
	
	for(auto& prop : props){
		PyObject* pyPropName = PyUnicode_FromString(prop.first.c_str());
		PyObject* pyProp = PyLong_FromVoidPtr((void*)prop.second);
		if(!pyPropName || !pyProp)
			return false;

		PyDict_SetItem(_propDict, pyPropName, pyProp);
	}
	
	for(auto& method : clientMethods){
		PyObject* pyPropName = PyUnicode_FromString(method.first.c_str());
		PyObject* pyProp = PyLong_FromVoidPtr((void*)method.second);
		if(!pyPropName || !pyProp)
			return false;
		
		PyDict_SetItem(_clientMethodDict, pyPropName, pyProp);
	}
	
	for(auto& method : cellMethods){
		PyObject* pyPropName = PyUnicode_FromString(method.first.c_str());
		PyObject* pyProp = PyLong_FromVoidPtr((void*)method.second);
		if(!pyPropName || !pyProp)
			return false;
		
		PyDict_SetItem(_cellMethodDict, pyPropName, pyProp);
	}
	
	for(auto& method : baseMethods){
		PyObject* pyPropName = PyUnicode_FromString(method.first.c_str());
		PyObject* pyProp = PyLong_FromVoidPtr((void*)method.second);
		if(!pyPropName || !pyProp)
			return false;
		
		PyDict_SetItem(_baseMethodDict, pyPropName, pyProp);
	}

	return initDefaultCellData();
}

bool ScriptDefModule::initDefaultCellData(){
	const PROPS_MAP& props = _objectDefModule->getProps();
	const char* moduleName = getModuleName();
	for(auto propIter : props){
		const IProp* prop = propIter.second;
		if(prop->getSetting(moduleName) & prop_def::ObjectDataFlagRelation::OBJECT_CELL_DATA_FLAGS){
			IDataType* dataType = (IDataType*)(prop->getExtra(moduleName));
			PyObject* value = (PyObject*)(dataType->parseDefaultStr(prop->getDefaultVal(moduleName)));
			PyDict_SetItemString(_cellPropData, prop->getNameString(), value); 
		}
	}
	return true;
}

PyObject* ScriptDefModule::scriptGetObjectAttribute(PyObject* object, PyObject* attr){
	EntityScriptObject* scriptObject = static_cast<EntityScriptObject*>(object);
    const IProp* prop = getProp(attr);
	if (!prop){
		return nullptr;
	}

	IObject* innerObject = scriptObject->getInnerObject();
	IDataType* dataType = (IDataType*)prop->getExtra(innerObject);
	return (PyObject*)dataType->createFromObject(innerObject, prop);
}

int32 ScriptDefModule::scriptSetObjectAttribute(PyObject* object, PyObject* attr, PyObject* value){
	EntityScriptObject* scriptObject = static_cast<EntityScriptObject*>(object);
    const IProp* prop = getProp(attr);
	if (!prop){
		return -1;
	}
	
	IObject* innerObject = scriptObject->getInnerObject();
	IDataType* dataType = (IDataType*)prop->getExtra(innerObject);
	dataType->addToObject(innerObject, prop, value);
	return 0;
}

const IProp* ScriptDefModule::getProp(PyObject* attr){
	PyObject* pyVal = PyDict_GetItem(_propDict, attr);
    wchar_t* pyUnicodeWideString = PyUnicode_AsWideCharString(attr, NULL);
    char* propName = sl::CStringUtils::wchar2char(pyUnicodeWideString);
    PyMem_Free(pyUnicodeWideString);
	printf("ddc PyObject propName:%s\n", propName);
	if (!pyVal){
		return nullptr;
	}
	return (const IProp*)PyLong_AsVoidPtr(pyVal);
}

const IProp* ScriptDefModule::getMethodProp(const int8 type, PyObject* attr){
    PyObject* methodDict = nullptr;
    switch(type){
    case EntityMailBoxType::MAILBOX_TYPE_BASE: methodDict = _baseMethodDict; break;
    case EntityMailBoxType::MAILBOX_TYPE_CELL: methodDict = _cellMethodDict; break;
    case EntityMailBoxType::MAILBOX_TYPE_CLIENT: methodDict = _clientMethodDict; break;
    case EntityMailBoxType::MAILBOX_TYPE_CELL_VIA_BASE: methodDict = _cellMethodDict; break;
    case EntityMailBoxType::MAILBOX_TYPE_BASE_VIA_CELL: methodDict = _baseMethodDict; break;
    case EntityMailBoxType::MAILBOX_TYPE_CLIENT_VIA_BASE: methodDict = _clientMethodDict; break;
    case EntityMailBoxType::MAILBOX_TYPE_CLIENT_VIA_CELL: methodDict = _clientMethodDict; break;
    default: break;
    } 
    
    PyObject* pyVal = PyDict_GetItem(methodDict, attr);
    if(!pyVal)
        return nullptr;
    return (const IProp*)PyLong_AsVoidPtr(pyVal);
}

bool ScriptDefModule::checkMethodArgs(IObject* object, const IProp* methodProp, PyObject* args){
    if(!args || !PyTuple_Check(args)){
        PyErr_Format(PyExc_AssertionError, "Method::checkArgs: method[%d] args is not a tuple.\n", methodProp->getName());
        PyErr_PrintEx(0);
        return false;
    }

    /*int64 argsType = methodProp->getExtra(object);
    int8* argType = (int8*)argsType;

    int32 pyArgsSize = PyTuple_Size(args);
    int32 i = 0;
    for(;i < pyArgsSize; ++i){
        PyObject* pyArg = PyTuple_GetItem(args, i);
        if(!DataTypeMgr::isDataType(pyArg, argType[i]))
            return false;
    }

    if(i >= (int32)(sizeof(int64)/sizeof(int8)) || argType[i] != 0xFF)
        return false;
*/
    return true;
}

void ScriptDefModule::initializeEntity(PyObject* object, PyObject* dictData){
	initPropDefaultValue(object);
    createNameSpace(object, dictData);
    initializeScript(object);
}

void ScriptDefModule::initPropDefaultValue(PyObject* object){
	const PROPS_MAP& props = _objectDefModule->getProps();	
	for(auto propIter : props){
		const IProp* prop = propIter.second;
		EntityScriptObject* scriptObject = static_cast<EntityScriptObject*>(object);
		IObject* innerObject = scriptObject->getInnerObject();
		IDataType* dataType = (IDataType*)(prop->getExtra(innerObject));
		const char* defaultVal = prop->getDefaultVal(innerObject);
		PyObject* pyDefaultVal = (PyObject*)(dataType->parseDefaultStr(defaultVal));
		dataType->addToObject(innerObject, prop, pyDefaultVal);
	}
}

void ScriptDefModule::createNameSpace(PyObject* object, PyObject* dictData){
    if(dictData == NULL)
        return;

    if(!PyDict_Check(dictData)){
        PyErr_Format(PyExc_AssertionError, "createNameSpace: create failed, args is not dict\n");
        PyErr_PrintEx(0);
        return;
    }

    Py_ssize_t pos = 0;
    PyObject *key, *value;
	PyObject* pyCellDataAttr = PyUnicode_FromString("cellData");
	//PyObject * cellDataDict = PyObject_GetAttrString(object, "cellData");
	PyObject* cellDataDict = ((EntityScriptObject*)object)->onScriptGetAttribute(pyCellDataAttr);
    if(cellDataDict == NULL)
        PyErr_Clear();

    while(PyDict_Next(dictData, &pos, &key, &value)){
        PyObject* val = scriptGetObjectAttribute(object, key);
        if(val != nullptr){
            scriptSetObjectAttribute(object, key, value);
            continue;
        }

        if(cellDataDict != NULL && PyDict_Contains(cellDataDict, key) > 0)
            PyDict_SetItem(cellDataDict, key, value);
        else
            PyObject_SetAttr(object, key, value);
    }

    SCRIPT_ERROR_CHECK();
    Py_XDECREF(cellDataDict);
}

void ScriptDefModule::initializeScript(PyObject* object){
	if (PyObject_HasAttrString(object, "__init__")){																									
		PyObject* pyResult = PyObject_CallMethod(object, const_cast<char*>("__init__"), 
		const_cast<char*>(""));											
		if (pyResult != NULL)																			
			Py_DECREF(pyResult);																		
		else																							
			SCRIPT_ERROR_CHECK();																		
	}																									
}

void ScriptDefModule::setDefaultCellData(PyObject* dataDict){
    Py_ssize_t pos = 0;
    PyObject *key, *value;
    while(PyDict_Next(_cellPropData, &pos, &key, &value)){
	    if (!value){
            PyErr_Format(PyExc_AssertionError, "SetDefaultCellData:getCellPropData Prop is null!\n");
            PyErr_PrintEx(0);
		    return;
	    }
	    //const IProp* cellProp = (const IProp*)PyLong_AsVoidPtr(value);
        //PyObject* pyValue = PyLong_FromLong(0);
        PyDict_SetItem(dataDict, key, value);
    }

    SCRIPT_ERROR_CHECK();
}

bool ScriptDefModule::createCellDataFromStream(PyObject* object, const void* cellData, const int32 cellDataSize){
	EntityScriptObject* scriptObject = static_cast<EntityScriptObject*>(object);
    IObject* entity = scriptObject->getInnerObject();
    
    sl::OBMap cellDataStream(cellData, cellDataSize);
    printf("cell entity %s has attr %d, datasize:%d\n", entity->getObjTypeString(), entity->getObjProps().size(), cellDataSize);
    Py_ssize_t pos = 0;
    PyObject *key, *value;
    while(PyDict_Next(_propDict, &pos, &key, &value)){
	    if (!value){
            PyErr_Format(PyExc_AssertionError, "createCellDataFromStream:getPropData Prop is null!\n");
            PyErr_PrintEx(0);
		    return false;
	    }
	    const IProp* prop = (const IProp*)PyLong_AsVoidPtr(value);
       // DataTypeMgr::readDataFromStream(entity, prop, cellDataStream);
    }
    return true;
}


const IProp* ScriptDefModule::findMethodProp(const uint16 methodIndex){
	const int32 nodeType = SLMODULE(Harbor)->getNodeType(); 
	if(nodeType == NodeType::LOGIC){
		auto itor = _idToBaseMethods.find(methodIndex);
		if(itor != _idToBaseMethods.end())
			return itor->second;
	}
	else if(nodeType == NodeType::SCENE){
		auto itor = _idToCellMethods.find(methodIndex);
		if(itor != _idToCellMethods.end())
			return itor->second;
	}

	return nullptr;
}

PyObject* ScriptDefModule::createArgsFromStream(IObject* object, const IProp* methodProp, sl::OBStream& stream){
	int32 argsCount = methodProp->getSize(object);
	PyObject* pyArgsTuple = NULL;
	int32 offset = 0;

	pyArgsTuple = PyTuple_New(argsCount);

	for(int32 index = 0; index < argsCount; ++index){
	}

	return pyArgsTuple;
}
