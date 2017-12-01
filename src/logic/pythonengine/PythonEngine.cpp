#include "PythonEngine.h"
#include "slstring_utils.h"
#include "slpymacros.h"
#define ENGINE_MODULE "shyloo"
bool PythonEngine::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	_self = this;
	const char* slResPath = "./shyloo";
	const char* slPyPath = "E:/shyloo/src/sllib/slpyscript/Lib;E:/server/res";
	installPyScript(slResPath, slPyPath);
	return true;
}

bool PythonEngine::launched(sl::api::IKernel * pKernel){
	APPEND_SCRIPT_MODULE_METHOD(this, "testPyCall", &PythonEngine::__py_testPyCall);
	test();
	return true;
}

bool PythonEngine::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

bool PythonEngine::installPyScript(const char* resPath, const char* userPath){
	bool ret = getScript().install(resPath, userPath, ENGINE_MODULE);
	return true;
}

void PythonEngine::appendScriptModuleMethod(PyCFunction f, const char* funcName){
	_scriptMethods[funcName] = { funcName, f, METH_VARARGS, NULL };
	PyModule_AddObject(getScript().getModule(), funcName, PyCFunction_New(&_scriptMethods[funcName], 0));
}

PyObject* PythonEngine::__py_testPyCall(PyObject* self, PyObject* args){
	ECHO_ERROR("PYTHON CALL BACK");
	return 0;
}

void PythonEngine::test(){
	PyRun_SimpleString("from ddddtest import htest\nhtest()\n");
}