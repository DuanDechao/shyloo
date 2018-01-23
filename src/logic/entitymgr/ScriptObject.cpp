#include "ScriptObject.h"
#include "IDCCenter.h"

ScriptObject::SCRIPTOBJECT_TYPES ScriptObject::s_allObjectTypes;
SCRIPT_METHOD_DECLARE_BEGIN(ScriptObject)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_MEMBER_DECLARE_BEGIN(ScriptObject)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(ScriptObject)
SCRIPT_GETSET_DECLARE_END()
SCRIPT_INIT(ScriptObject, 0, 0, 0, 0, 0)

ScriptObject::ScriptObject(PyTypeObject* pyType, const char* objectName, const uint64 objectId, bool isInitialised)
    :_innerObject(nullptr)
{
	if (PyType_Ready(pyType) < 0){
		ECHO_ERROR("ScriptObject: Type %s is not ready\n", pyType->tp_name);
	}

	if (!isInitialised){
		PyObject_INIT(static_cast<PyObject*>(this), pyType);
	}
    
    if(objectId != 0){
        IObject* object = CREATE_OBJECT_BYID(SLMODULE(ObjectMgr), objectName, objectId);
        setInnerObject(object);
    }
}

ScriptObject::~ScriptObject(){
	SLASSERT(this->ob_refcnt == 0, "ref leak");
}

PyTypeObject* ScriptObject::getScriptObjectType(const std::string& name){
	ScriptObject::SCRIPTOBJECT_TYPES::iterator iter = s_allObjectTypes.find(name);
	if (iter != s_allObjectTypes.end()){
		return iter->second;
	}

	return NULL;
}

PyObject* ScriptObject::tp_repr(){
	return PyUnicode_FromFormat("%s object at %p.", this->scriptName(), this);
}

PyObject* ScriptObject::tp_str(){
	return PyUnicode_FromFormat("%s object at %p.", this->scriptName(), this);
}

PyObject* ScriptObject::tp_new(PyTypeObject* type, PyObject* args, PyObject* kwds){
	return type->tp_alloc(type, 0);
}

//�ű������ȡ���Ի򷽷�
PyObject* ScriptObject::onScriptGetAttribute(PyObject* attr){
	return PyObject_GenericGetAttr(this, attr);
}

//�ű������������Ի򷽷�
int ScriptObject::onScriptSetAttribute(PyObject* attr, PyObject* value){
	return PyObject_GenericSetAttr(static_cast<PyObject*>(this), attr, value);
}

//�ű�����ɾ��һ������
int ScriptObject::onScriptDelAttribute(PyObject* attr){
	return this->onScriptSetAttribute(attr, NULL);
}

void ScriptObject::initializeScript(){
	if (PyObject_HasAttrString(this, "__init__")){																									
		PyObject* pyResult = PyObject_CallMethod(this, const_cast<char*>("__init__"), 
		const_cast<char*>(""));											
		if (pyResult != NULL)																			
			Py_DECREF(pyResult);																		
		else																							
			SCRIPT_ERROR_CHECK();																		
	}																									
}
