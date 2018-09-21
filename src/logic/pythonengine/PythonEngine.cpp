#include "PythonEngine.h"
#include "IResMgr.h"
#include "NodeProtocol.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "pyscript/py_gc.h"
#include "pyscript/pickler.h"
#include "IDebugHelper.h"
PythonEngine* PythonEngine::s_self = nullptr;
bool PythonEngine::initialize(sl::api::IKernel * pKernel){
	s_self = this;
	_kernel = pKernel;

	return initPyEnvir(pKernel);
}

bool PythonEngine::launched(sl::api::IKernel * pKernel){
    return true;
}

bool PythonEngine::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

bool PythonEngine::initPyEnvir(sl::api::IKernel* pKernel){
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
	return getScript().install(userResPath.c_str(), pyPaths.c_str(), "shyloo");
}

void PythonEngine::installScriptModuleMethod(const char* funcName, PyCFunction f){
	_scriptMethods[funcName] = { funcName, f, METH_VARARGS, NULL };
	PyModule_AddObject(getScript().getBaseModule(), funcName, PyCFunction_New(&_scriptMethods[funcName], 0));
}

void PythonEngine::addScriptIntConstant(const char* varName, int32 value){
	PyModule_AddIntConstant(getScript().getBaseModule(), varName, value);
}

int32 PythonEngine::registerPyObjectToScript(const char* attrName, PyObject* pyObj){
	return PyObject_SetAttrString(getScript().getBaseModule(), attrName, pyObj);
}

int32 PythonEngine::unregisterPyObjectToScript(const char* attrName){
	if(!attrName)
		return 0;
	return PyObject_DelAttrString(getScript().getBaseModule(), attrName);
}

void PythonEngine::installScriptModuleType(PyTypeObject* scriptType, const char* typeName){
	if (PyType_Ready(scriptType) < 0){
		ECHO_ERROR("PyType_Ready %s is error", typeName);
		PyErr_Print();
		return;
	}
	Py_INCREF(scriptType);
	if (PyModule_AddObject(getScript().getBaseModule(), typeName, (PyObject*)scriptType) < 0){
		ECHO_ERROR("PyModule_AddObject(%s) is error!", typeName);
	}																				
	if (PyErr_Occurred()){
		PyErr_PrintEx(0);
	}
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

bool PythonEngine::runSimpleString(std::string command, std::string& ret){
	if(command.size() == 0)
		return false;

	command += "\n";
	PyObject* pycmd = PyUnicode_DecodeUTF8(command.data(), command.size(), NULL);
	if(!pycmd){
		if(PyErr_Occurred()){
			PyErr_PrintEx(0);
		}
		return false;
	}

	PyObject* pycmd1 = PyUnicode_AsEncodedString(pycmd, "utf-8", NULL);
	if(!_pyScript.runSimpleString(PyBytes_AsString(pycmd1), &ret)){
		Py_DECREF(pycmd);
		Py_DECREF(pycmd1);
		return false;
	}

	Py_DECREF(pycmd);
	Py_DECREF(pycmd1);
	return true;
}

