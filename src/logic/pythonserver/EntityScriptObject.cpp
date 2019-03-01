#include "EntityScriptObject.h"
#include "IDCCenter.h"
#include "PythonServer.h"
#include "IHarbor.h"
SCRIPT_METHOD_DECLARE_BEGIN(EntityScriptObject)
SCRIPT_METHOD_DECLARE("addTimer",  addTimer,        METH_VARARGS,   0)
SCRIPT_METHOD_DECLARE("delTimer",  delTimer,        METH_VARARGS,   0)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_MEMBER_DECLARE_BEGIN(EntityScriptObject)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(EntityScriptObject)
SCRIPT_GET_DECLARE("id",              pyGetID,            0,              0)
SCRIPT_GETSET_DECLARE_END()

BASE_SCRIPT_INIT(EntityScriptObject, 0, 0, 0, 0, 0)
EntityScriptObject::EntityScriptObject(PyTypeObject* pyType, IObject* object, ScriptDefModule* pScriptModule, bool isInitialised)
    :ScriptObject(pyType, isInitialised),
	 _innerObject(nullptr),
     _pScriptModule(pScriptModule)
{
    setInnerObject(object);
}

EntityScriptObject::~EntityScriptObject(){
}

PyObject* EntityScriptObject::pyGetID(){
	return PyLong_FromUnsignedLongLong(getInnerObject()->getID());
}

PyObject* EntityScriptObject::addTimer(float delay, float interval, const char* callbackName, PyObject* userData){
	int64 timerId = _scriptTimers.addTimer(PythonServer::getInstance()->getKernel(), this, (int64)(delay * 1000), (int64)(interval * 1000), callbackName, userData);
	return PyLong_FromUnsignedLongLong(timerId);
}

PyObject* EntityScriptObject::delTimer(int32 timerId){
	_scriptTimers.delTimer(PythonServer::getInstance()->getKernel(), this, timerId);
	S_Return;
}


PyObject* EntityScriptObject::onScriptGetAttribute(PyObject* attr){
//	PyObject* pyValue = ScriptObject::onScriptGetAttribute(attr);
//	if(PyErr_Occurred() && !pyValue){
//		PyErr_Clear();
//	}
//	if(!pyValue){
	wchar_t* pyUnicodeWideString = PyUnicode_AsWideCharString(attr, NULL);
	char* keyStr = sl::CStringUtils::wchar2char(pyUnicodeWideString);
	PyMem_Free(pyUnicodeWideString);
	PyObject* ret = _pScriptModule->scriptGetObjectAttribute(this, attr);
	return ret;
//		ScriptObject::onScriptSetAttribute(attr, pyValue);
//	}
//	return pyValue;
}

int EntityScriptObject::onScriptSetAttribute(PyObject* attr, PyObject* value){
	//int32 ret = ScriptObject::onScriptSetAttribute(attr, value);
	return _pScriptModule->scriptSetObjectAttribute(this, attr, value);
}


void EntityScriptObject::onRemoteMethodCall(const sl::OBStream& stream){
	uint16 methodIndex = 0;
	if(!stream.readUint16(methodIndex))
		return;

	const IProp* methodProp = _pScriptModule->findMethodProp(methodIndex);
	if(!methodProp){
		printf("[%d]method[%d] is not exist!\n",SLMODULE(Harbor)->getNodeType(), methodIndex);
		return;
	}
	
	ECHO_TRACE("onRemoteMethodCall from remote call method[%d][%s]", methodIndex, methodProp->getNameString());
	
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
	if(!pyResult){
		SCRIPT_ERROR_CHECK();
	}
	else{
		Py_XDECREF(pyResult);
	}

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
		PyObject* pyitem = (PyObject*)(argsType[index]->createScriptObject(stream));
		if(pyitem == NULL){
			ECHO_ERROR("MethodDescription::createFromStream: %s arg[%d][%s] is NULL.\n", 
				_pScriptModule->getModuleName(), index, argsType[index]->getName());
		}

		PyTuple_SET_ITEM(pyArgsTuple, index + offset, pyitem);
	}
	
	return pyArgsTuple;
}

