#ifndef __SL_INTERFACE_PYTHON_ENGINE_H__
#define __SL_INTERFACE_PYTHON_ENGINE_H__
#include "slimodule.h"
#include "slpyscript.h"
class IPythonEngine : public sl::api::IModule{
public:
	virtual ~IPythonEngine() {}

	virtual void installScriptModuleMethod(PyCFunction f, const char* pyFuncName) = 0;
	virtual void installScriptModuleType(PyTypeObject* scriptType, const char* typeName) = 0;
};

#define INSTALL_SCRIPT_MODULE_METHOD(pyEngine, METHOD_NAME, f) pyEngine->installScriptModuleMethod(f, METHOD_NAME)
#define INSTALL_SCRIPT_MODULE_TYPE(pyEngine, TYPE_NAME, OBJ_TYPE) pyEngine->installScriptModuleType(OBJ_TYPE, TYPE_NAME)
#endif