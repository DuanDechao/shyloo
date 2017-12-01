#ifndef __SL_CORE_PYTHON_ENGINE_H__
#define __SL_CORE_PYTHON_ENGINE_H__
#include "slikernel.h"
#include "IPythonEngine.h"
#include "slpyscript.h"
#include <unordered_map>
#include <string>
using namespace sl;

class PythonEngine : public IPythonEngine{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	bool installPyScript(const char* resPath, const char* userPath);
	virtual void appendScriptModuleMethod(PyCFunction f, const char* funcName);

	inline script::PyScript& getScript() { return _script; }

	static PyObject* __py_testPyCall(PyObject* self, PyObject* args);

	void test();

private:
	sl::api::IKernel*			_kernel;
	PythonEngine*				_self;
	sl::script::PyScript		_script;
	std::unordered_map<std::string, PyMethodDef> _scriptMethods;

};
#endif