#include "slpyscript.h"
#include "slstring_utils.h"
#include "IResMgr.h"
#include "pyscript/pickler.h"
#include "pyscript/py_gc.h"
namespace sl{
SL_SINGLETON_INIT(script::PyScript);
namespace script{
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
	wchar_t* pwpySysResPath = CStringUtils::char2wchar(pythonHomeDir);
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
	wchar_t* pwpyUserPath = CStringUtils::char2wchar(pyUserPaths.c_str());
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
	_module = PyImport_AddModule(moduleName);
	if (NULL == _module)
		return false;

	PyObject_SetAttrString(m, moduleName, _module);
	PyObject_SetAttrString(_module, "__doc__", PyUnicode_FromString("This module is created by shyloo!"));
	
	sl::pyscript::Pickler::initialize();
	sl::pyscript::PyGC::initialize();

	return true;
}
}
}
