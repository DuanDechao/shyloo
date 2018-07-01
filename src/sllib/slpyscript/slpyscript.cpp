#include "slpyscript.h"
#include "slstring_utils.h"
namespace sl{
SL_SINGLETON_INIT(script::PyScript);
namespace script{
bool PyScript::install(const char* pythonHomeDir, const char* pyPaths, const char* moduleName){
	
	//设置python的环境变量
	wchar_t* pwpySysResPath = CStringUtils::char2wchar(pythonHomeDir);
	Py_SetPythonHome(pwpySysResPath);
	free(pwpySysResPath);

	wchar_t* pwpyUserPath = CStringUtils::char2wchar(pyPaths);
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

	return true;
}
}
}