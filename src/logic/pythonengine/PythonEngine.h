#ifndef SL_LOGIC_PYTHON_ENGINE_H
#define SL_LOGIC_PYTHON_ENGINE_H
#include "slikernel.h"
#include "IPythonEngine.h"
#include <unordered_map>
#include "PyScript.h"
#include "slsingleton.h"
class PythonEngine : public IPythonEngine, public sl::SLHolder<PythonEngine>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	bool installPyScript(const char* resPath, const char* userPath);
	virtual void installScriptModuleMethod(PyCFunction f, const char* funcName);
	virtual void installScriptModuleType(PyTypeObject* scriptType, const char* typeName);
	virtual void installScriptModuleMethod(const char* funcName, PyCFunction f);
	virtual void installScriptModuleType(PyTypeObject* scriptType, const char* typeName);
	virtual PyObject* getBaseModule() {return getScript().getBaseModule();}
	virtual PyObject* getMathModule() {return getScript().getMathModule();}
	virtual std::string pickle(PyObject* pyobj);
	virtual PyObject* unpickle(const std::string& str);
	virtual void registerUnpickleFunc(PyObject* pyFunc, const char* funcName);
	virtual PyObject* getUnpickleFunc(const char* funcName);
	virtual int32 registerPyObjectToScript(const char* attrName, PyObject* pyObj);
	virtual int32 unregisterPyObjectToScript(const char* attrName);
	virtual void incTracing(std::string name);
	virtual void decTracing(std::string name);
	virtual void addScriptIntConstant(const char* varName, int32 value);
	virtual bool runSimpleString(std::string command, std::string& ret);

private:
	bool initPyEnvir(sl::api::IKernel* pKernel);
	inline PyScript& getScript() {return _pyScript;}

private:
	static PythonEngine*								s_self;
	sl::api::IKernel*									_kernel;
	PyScript											_pyScript;
	std::unordered_map<std::string, PyMethodDef>		_scriptMethods;
};
#endif
