#ifndef __SL_INTERFACE_PYTHON_ENGINE_H__
#define __SL_INTERFACE_PYTHON_ENGINE_H__
#include "slimodule.h"
#include "slpyscript.h"
class IPythonEngine : public sl::api::IModule{
public:
	virtual ~IPythonEngine() {}

	virtual void appendScriptModuleMethod(PyCFunction f, const char* pyFuncName) = 0;
};

#define APPEND_SCRIPT_MODULE_METHOD(pyEngine, METHOD_NAME, f) pyEngine->appendScriptModuleMethod(f, METHOD_NAME)
#endif