#include "slscript_object.h"
namespace sl{
namespace script{
ScriptObject::SCRIPTOBJECT_TYPES ScriptObject::s_allObjectTypes;
SCRIPT_METHOD_DECLARE_BEGIN(ScriptObject)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_MEMBER_DECLARE_BEGIN(ScriptObject)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(ScriptObject)
SCRIPT_GETSET_DECLARE_END()
SCRIPT_INIT(ScriptObject, 0, 0, 0, 0, 0)

ScriptObject::ScriptObject(PyTypeObject* pyType, bool isInitialised){
	if (PyType_Ready(pyType) < 0){
		ECHO_ERROR("ScriptObject: Type %s is not ready\n", pyType->tp_name);
	}

	if (!isInitialised){
		PyObject_INIT(static_cast<PyObject*>(this), pyType);
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

//脚本请求获取属性或方法
PyObject* ScriptObject::onScriptGetAttribute(PyObject* attr){
	return PyObject_GenericGetAttr(this, attr);
}

//脚本请求设置属性或方法
int ScriptObject::onScriptSetAttribute(PyObject* attr, PyObject* value){
	return PyObject_GenericSetAttr(static_cast<PyObject*>(this), attr, value);
}

//脚本请求删除一个属性
int ScriptObject::onScriptDelAttribute(PyObject* attr){
	return this->onScriptSetAttribute(attr, NULL);
}

}
}
