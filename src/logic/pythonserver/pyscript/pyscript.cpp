#include "pyscript.h"
#include "slstring_utils.h"
#include "pickler.h"
#include "py_gc.h"
#define SCRIPT_PATH																		\
					"../../../server/res/server/scripts;"								\
					"../../../server/res/server/scripts/common;"						\
					"../../../server/res/server/scripts/common/lib-dynload;"			\
					"../../../server/res/server/scripts/common/DLLs;"					\
					"../../../server/res/server/scripts/common/Lib;"					\
					"../../../server/res/server/scripts/common/Lib/site-packages;"		\
					"../../../server/res/server/scripts/common/Lib/dist-packages"
					
bool PyScript::install(const char* pythonHomeDir, const char* pyPaths, const char* moduleName){
	//设置python的环境变量
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

	//python解释器初始化
	Py_Initialize();
	if (!Py_IsInitialized()){
		ECHO_ERROR("Script::install(): Py_Initialize is failed!\n");
		return false;
	}

	PyObject* m = PyImport_AddModule("__main__");

	//添加一个脚本基础模块
	_baseModule = PyImport_AddModule(moduleName);
	if (NULL == _baseModule)
		return false;

	PyObject_SetAttrString(m, moduleName, _baseModule);
	PyObject_SetAttrString(_baseModule, "__doc__", PyUnicode_FromString("This module is created by shyloo!"));
	
	//添加一个脚本数学模块
	_mathModule = PyImport_AddModule("Math");
	if( NULL == _mathModule)
		return false;

	PyObject_SetAttrString(m, moduleName, _mathModule);
	PyObject_SetAttrString(_mathModule, "__doc__", PyUnicode_FromString("This module is created by shyloo!"));
	
	sl::pyscript::Pickler::initialize();
	sl::pyscript::PyGC::initialize();

	return true;
}
