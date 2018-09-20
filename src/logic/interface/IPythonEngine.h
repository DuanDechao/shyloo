#ifndef _SL_INTERFACE_PYTHONENGINE_H__
#define _SL_INTERFACE_PYTHONENGINE_H__
#include "slimodule.h"
#include "Python.h"
class IPythonEngine : public sl::api::IModule{
public:
	virtual ~IPythonEngine() {}
	virtual void installScriptModuleMethod(const char* funcName, PyCFunction f) = 0;
	virtual void installScriptModuleType(PyTypeObject* scriptType, const char* typeName) = 0;
	virtual PyObject* getBaseModule() = 0;
	virtual PyObject* getMathModule() = 0;
	virtual std::string pickle(PyObject* pyobj) = 0;
	virtual PyObject* unpickle(const std::string& str) = 0;
	virtual void registerUnpickleFunc(PyObject* pyFunc, const char* funcName) = 0;
	virtual PyObject* getUnpickleFunc(const char* funcName) = 0;
	virtual int32 registerPyObjectToScript(const char* attrName, PyObject* pyObj)= 0;
	virtual int32 unregisterPyObjectToScript(const char* attrName) = 0;
	virtual void incTracing(std::string name) = 0;
	virtual void decTracing(std::string name) = 0;
	virtual void addScriptIntConstant(const char* varName, int32 value) = 0;
	virtual bool runSimpleString(std::string command, std::string& ret) = 0;
};

#endif
