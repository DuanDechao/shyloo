#include "PythonEngine.h"
#include "slstring_utils.h"
#include "slpymacros.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "IResMgr.h"
#include "pyscript/pickler.h"
#include "pyscript/py_gc.h"
#define ENGINE_MODULE "shyloo"
					
bool PythonEngine::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	_self = this;

	if(SLMODULE(ResMgr)->getPyUserScriptsPath().size() == 0){
		ERROR_LOG("PythonEngine::initialize: SL_RES_PATH error");
		return false;
	}
	
	std::string userScriptsPath = SLMODULE(ResMgr)->getPyUserScriptsPath();
	std::string pyPaths = userScriptsPath + "common;";

    if(SLMODULE(Harbor)->getNodeType() == NodeType::LOGIC){
        pyPaths += userScriptsPath + "base;";
        pyPaths += userScriptsPath + "base/interfaces;";
	}
    if(SLMODULE(Harbor)->getNodeType() == NodeType::SCENE){
        pyPaths += userScriptsPath + "cell;";
        pyPaths += userScriptsPath + "cell/interfaces;";
	}
    pyPaths += userScriptsPath + "common_server;";
    pyPaths += userScriptsPath + "common/data;";
	
	std::string userResPath = SLMODULE(ResMgr)->getPyUserResPath();
	userResPath += "server/scripts/common";
    installPyScript(userResPath.c_str(), pyPaths.c_str());
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

int32 PythonEngine::registerPyObjectToScript(const char* attrName, PyObject* pyObj){
	return PyObject_SetAttrString(getScript().getModule(), attrName, pyObj);
}

int32 PythonEngine::unregisterPyObjectToScript(const char* attrName){
	if(!attrName)
		return 0;
	return PyObject_DelAttrString(getScript().getModule(), attrName);
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

std::string PythonEngine::pickle(PyObject* pyobj){
	return sl::pyscript::Pickler::pickle(pyobj);
}

PyObject* PythonEngine::unpickle(const std::string& str){
	return sl::pyscript::Pickler::unpickle(str);
}

void PythonEngine::registerUnpickleFunc(PyObject* pyFunc, const char* funcName){
	sl::pyscript::Pickler::registerUnpickleFunc(pyFunc, funcName);
}

PyObject* PythonEngine::getUnpickleFunc(const char* funcName){
	return sl::pyscript::Pickler::getUnpickleFunc(funcName);
}

void PythonEngine::incTracing(std::string name){
	sl::pyscript::PyGC::incTracing(name);
}

void PythonEngine::decTracing(std::string name){
	sl::pyscript::PyGC::decTracing(name);
}
void PythonEngine::test(){
	PyRun_SimpleString("from ddddtest import htest\nhtest()\n");
}
