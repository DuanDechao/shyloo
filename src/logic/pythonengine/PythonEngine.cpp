#include "PythonEngine.h"
#include "slstring_utils.h"
#include "slpymacros.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#define ENGINE_MODULE "shyloo"
bool PythonEngine::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	_self = this;
	const char* slResPath = "/home/duandechao/shyloo/build/linux";
	const char* slPyPath = nullptr;
    if(SLMODULE(Harbor)->getNodeType() == NodeType::LOGIC)
        slPyPath = "/home/duandechao/shyloo/build/linux/Lib:/home/duandechao/shyloo/build/server/res:/home/duandechao/shyloo/build/server/res/entities/base";
    if(SLMODULE(Harbor)->getNodeType() == NodeType::SCENE)
        slPyPath = "/home/duandechao/shyloo/build/linux/Lib:/home/duandechao/shyloo/build/server/res/entities/cell";
	
    installPyScript(slResPath, slPyPath);
	return true;
}

bool PythonEngine::launched(sl::api::IKernel * pKernel){
	INSTALL_SCRIPT_MODULE_METHOD(this, "testPyCall", &PythonEngine::__py_testPyCall);
	//test();
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

void PythonEngine::installScriptModuleMethod(PyCFunction f, const char* funcName){
	_scriptMethods[funcName] = { funcName, f, METH_VARARGS, NULL };
	PyModule_AddObject(getScript().getModule(), funcName, PyCFunction_New(&_scriptMethods[funcName], 0));
}

void PythonEngine::installScriptModuleType(PyTypeObject* scriptType, const char* typeName){
	if (PyType_Ready(scriptType) < 0){
		ECHO_ERROR("PyType_Ready %s is error", typeName);
		PyErr_Print();
		return;
	}
	Py_INCREF(scriptType);
	if (PyModule_AddObject(getScript().getModule(), typeName, (PyObject*)scriptType) < 0){
		ECHO_ERROR("PyModule_AddObject(%s) is error!", typeName);
	}																				
	SCRIPT_ERROR_CHECK();															
}

PyObject* PythonEngine::__py_testPyCall(PyObject* self, PyObject* args){
	ECHO_ERROR("PYTHON CALL BACK");
	return 0;
}

void PythonEngine::test(){
	PyRun_SimpleString("from ddddtest import htest\nhtest()\n");
}
