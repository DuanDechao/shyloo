#include "EntityScriptObject.h"
#include "IDCCenter.h"
#include "EntityMgr.h"
EntityScriptObject::EntityScriptObject(PyTypeObject* pyType, IObject* object, ScriptDefModule* pScriptModule, bool isInitialised)
    :ScriptObject(pyType, isInitialised),
	 _innerObject(nullptr),
     _pScriptModule(pScriptModule)
{
    setInnerObject(object);
}

EntityScriptObject::~EntityScriptObject(){
}


PyObject* EntityScriptObject::onScriptGetAttribute(PyObject* attr){
	PyObject* pyValue = ScriptObject::onScriptGetAttribute(attr);
	if(!pyValue){
		pyValue = _pScriptModule->scriptGetObjectAttribute(this, attr);
		ScriptObject::onScriptSetAttribute(attr, pyValue);
	}
	return pyValue;
}

int EntityScriptObject::onScriptSetAttribute(PyObject* attr, PyObject* value){
	int32 ret = ScriptObject::onScriptSetAttribute(attr, value);
	_pScriptModule->scriptSetObjectAttribute(this, attr, value);
	return ret;
}


void EntityScriptObject::onRemoteMethodCall(const sl::OBStream& stream){
	uint16 methodIndex = 0;
	if(!stream.readUint16(methodIndex))
		return;

	ECHO_TRACE("onRemoteMethodCall from remote call method[%d]", methodIndex);
	
	const IProp* methodProp = _pScriptModule->findMethodProp(methodIndex);
	if(!methodProp){
		printf("method[%d] is not exist!\n", methodIndex);
		return;
	}
	
	PyObject* pyFunc = PyObject_GetAttrString(this, const_cast<char*>(methodProp->getNameString()));
	PyObject* pyResult = NULL;
	if(!PyCallable_Check(pyFunc)){
		PyErr_Format(PyExc_TypeError, "EntityScriptObject::onRemoteMethodCall: method(%s) call attempted on a error object!", methodProp->getNameString());
		PyErr_PrintEx(0);
		Py_XDECREF(pyFunc);
		return;
	}

	PyObject* args = createArgsPyObjectFromStream(stream, methodProp);
	pyResult = PyObject_CallObject(pyFunc, args);
	if(args){
		Py_XDECREF(args);
	}
	Py_XDECREF(pyFunc);
}

PyObject* EntityScriptObject::createArgsPyObjectFromStream(const sl::OBStream& stream, const IProp* methodProp){
	IDataType** argsType = (IDataType**)methodProp->getExtra(_innerObject);
	int32 argSize = methodProp->getSize(_innerObject);
	if(argSize <= 0)
		return NULL;

	PyObject* pyArgsTuple = NULL;
	int offset = 0;
	
	/*if(isExposed() && g_componentType == CELLAPP_TYPE && isCell())
	{
		offset = 1;
		pyArgsTuple = PyTuple_New(argSize + offset);

		// 设置一个调用者ID提供给脚本判断来源是否正确
		KBE_ASSERT(currCallerID_ > 0);
		PyTuple_SET_ITEM(pyArgsTuple, 0, PyLong_FromLong(currCallerID_));
	}
	else*/
	pyArgsTuple = PyTuple_New(argSize);

	for(int32 index=0; index<argSize; ++index){
		PyObject* pyitem = (PyObject*)(argsType[index]->createFromStream(stream));
		if(pyitem == NULL){
			ECHO_ERROR("MethodDescription::createFromStream: %s arg[%d][%s] is NULL.\n", 
				_pScriptModule->getModuleName(), index, argsType[index]->getName());
		}

		PyTuple_SET_ITEM(pyArgsTuple, index + offset, pyitem);
	}
	
	return pyArgsTuple;
}


int64 EntityScriptObject::addTimer(int64 period, int64 interval, const char* callBackName, PyObject* userData){
	return _scriptTimers.addTimer(EntityMgr::getInstance()->getKernel(), this, period, interval, callBackName, userData);
}

void EntityScriptObject::delTimer(int32 timerId){
	_scriptTimers.delTimer(EntityMgr::getInstance()->getKernel(), this, timerId);
}
