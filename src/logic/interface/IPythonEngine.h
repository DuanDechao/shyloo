#ifndef __SL_INTERFACE_PYTHON_ENGINE_H__
#define __SL_INTERFACE_PYTHON_ENGINE_H__
#include "Python.h"
//#include "slpyscript.h"
#include "slimodule.h"
class IPythonEngine : public sl::api::IModule{
public:
	virtual ~IPythonEngine() {}

	virtual void installScriptModuleMethod(PyCFunction f, const char* pyFuncName) = 0;
	virtual void installScriptModuleType(PyTypeObject* scriptType, const char* typeName) = 0;
	virtual PyObject* getPythonModule()  = 0;
	virtual std::string pickle(PyObject* pyobj) = 0; 
	virtual PyObject* unpickle(const std::string& str) = 0;
	virtual void registerUnpickleFunc(PyObject* pyFunc, const char* funcName) = 0;
	virtual PyObject* getUnpickleFunc(const char* funcName) = 0;
	virtual int32 registerPyObjectToScript(const char* attrName, PyObject* pyObj) = 0;
	virtual int32 unregisterPyObjectToScript(const char* attrName) = 0;
	virtual void incTracing(std::string name) =0;
	virtual void decTracing(std::string name) =0;
};

#define INSTALL_SCRIPT_MODULE_METHOD(pyEngine, METHOD_NAME, f) pyEngine->installScriptModuleMethod(f, METHOD_NAME)
#define INSTALL_SCRIPT_MODULE_TYPE(pyEngine, TYPE_NAME, OBJ_TYPE) pyEngine->installScriptModuleType(OBJ_TYPE, TYPE_NAME)
#endif
