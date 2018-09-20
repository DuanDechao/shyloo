#include "PyScript.h"
#include "slstring_utils.h"
#include "IResMgr.h"
#include "pyscript/pickler.h"
#include "pyscript/py_gc.h"
#include "pyscript/scriptstdouterrhook.h"
#define SCRIPT_PATH																		\
					"../../../server/res/server/scripts;"								\
					"../../../server/res/server/scripts/common;"						\
					"../../../server/res/server/scripts/common/lib-dynload;"			\
					"../../../server/res/server/scripts/common/DLLs;"					\
					"../../../server/res/server/scripts/common/Lib;"					\
					"../../../server/res/server/scripts/common/Lib/site-packages;"		\
					"../../../server/res/server/scripts/common/Lib/dist-packages"
					
bool PyScript::install(const char* pythonHomeDir, const char* pyPaths, const char* moduleName){
	//����python�Ļ�������
    printf("%s\n", pythonHomeDir);
	wchar_t* pwpySysResPath = sl::CStringUtils::char2wchar(pythonHomeDir);
	Py_SetPythonHome(pwpySysResPath);
	free(pwpySysResPath);

	std::string pyUserPaths = pyPaths;
	std::string pySysPath = SCRIPT_PATH;
	std::string pyUserResPath = SLMODULE(ResMgr)->getPyUserResPath();
	sl::CStringUtils::RepleaceAll(pySysPath, "../../../server/res/", pyUserResPath);
	pyUserPaths += pySysPath;

#ifndef SL_OS_WINDOWS
	sl::CStringUtils::RepleaceAll(pyUserPaths, ";", ":");
#endif

    printf("%s\n", pyUserPaths.c_str());
	wchar_t* pwpyUserPath = sl::CStringUtils::char2wchar(pyUserPaths.c_str());
	Py_SetPath(pwpyUserPath);
	free(pwpyUserPath);

	//python��������ʼ��
	Py_Initialize();
	if (!Py_IsInitialized()){
		ECHO_ERROR("Script::install(): Py_Initialize is failed!\n");
		return false;
	}

	PyObject* m = PyImport_AddModule("__main__");

	//���һ���ű�����ģ��
	_baseModule = PyImport_AddModule(moduleName);
	if (NULL == _baseModule)
		return false;

	PyObject_SetAttrString(m, moduleName, _baseModule);
	PyObject_SetAttrString(_baseModule, "__doc__", PyUnicode_FromString("This module is created by shyloo!"));
	
	//���һ���ű���ѧģ��
	_mathModule = PyImport_AddModule("Math");
	if( NULL == _mathModule)
		return false;

	PyObject_SetAttrString(m, moduleName, _mathModule);
	PyObject_SetAttrString(_mathModule, "__doc__", PyUnicode_FromString("This module is created by shyloo!"));
	
	sl::pyscript::Pickler::initialize();
	sl::pyscript::PyGC::initialize();

	sl::pyscript::ScriptStdErr::installScript(NULL);
	sl::pyscript::ScriptStdOut::installScript(NULL);

	return true;
}

bool PyScript::runSimpleString(const char* command, std::string* retPtr){
	if(command == NULL)
		return false;

	if(retPtr != NULL){
		sl::pyscript::ScriptStdOutErrHook* pStdOutErrHook = NEW sl::pyscript::ScriptStdOutErrHook();
		if(!pStdOutErrHook->install()){
			printf("Script::runSimpleString: outErrHook install failed\n");
			SCRIPT_ERROR_CHECK();
			DEL pStdOutErrHook;
			return false;
		}

		pStdOutErrHook->setHookBuffer(retPtr);

		PyObject *m, *d, *v;
		m = PyImport_AddModule("__main__");
		if(m == NULL){
			SCRIPT_ERROR_CHECK();
			pStdOutErrHook->uninstall();
			DEL pStdOutErrHook;
			return false;
		}

		d = PyModule_GetDict(m);
		v = PyRun_String(command, Py_single_input, d, d);
		if( v == NULL ){
			PyErr_Print();
			pStdOutErrHook->uninstall();
			DEL pStdOutErrHook;
			return false;
		}

		Py_DECREF(v);
		SCRIPT_ERROR_CHECK();

		pStdOutErrHook->uninstall();
		DEL pStdOutErrHook;
		return true; 
	}

	PyRun_SimpleString(command);
	SCRIPT_ERROR_CHECK();
	return true;
}

