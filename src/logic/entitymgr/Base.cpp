#include "Base.h"
#include "IEntityDef.h"
#include "slscript_object.h"
#include "IPythonEngine.h"
#include "slstring_utils.h"

SCRIPT_METHOD_DECLARE_BEGIN(Base)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_MEMBER_DECLARE_BEGIN(Base)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(Base)
SCRIPT_GETSET_DECLARE_END()

BASE_SCRIPT_INIT(Base, 0, 0, 0, 0, 0)
Base::Base(int32 entityId, IScriptDefModule* pScriptModule, PyTypeObject* pyType, bool isInitialised)
	:_id(entityId),
	_pScriptModule(pScriptModule),
	sl::script::ScriptObject(pyType, isInitialised)
{}

Base::~Base(){
	SLASSERT(false, "wtf");
}

void Base::installScript(const char* name){
	refreshObjectType(name);
	INSTALL_SCRIPT_MODULE_TYPE(SLMODULE(PythonEngine), "Base", &s_objectType);
}

PyObject* Base::onScriptGetAttribute(PyObject* attr){
	PyObject* pyValue = _pScriptModule->scriptGetObjectAttribute(this, attr);
	if (!pyValue)
		return ScriptObject::onScriptGetAttribute(attr);
	return pyValue;
}

int Base::onScriptSetAttribute(PyObject* attr, PyObject* value){
	int32 ret = 0;
	if (!_pScriptModule || (ret = _pScriptModule->scriptSetObjectAttribute(this, attr, value)) < 0)
		return ScriptObject::onScriptSetAttribute(attr, value);
	return ret;
}

void Base::initializeScript(){
	if (PyObject_HasAttrString(this, "__init__")){																									
		PyObject* pyResult = PyObject_CallMethod(this, const_cast<char*>("__init__"), 
		const_cast<char*>(""));											
		if (pyResult != NULL)																			
			Py_DECREF(pyResult);																		
		else																							
			SCRIPT_ERROR_CHECK();																		
	}																									
}																										

