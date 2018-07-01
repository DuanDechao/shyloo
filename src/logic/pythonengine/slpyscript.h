#ifndef SL_PY_SCRIPT_SCRIPT_H
#define SL_PY_SCRIPT_SCRIPT_H
#include "slsingleton.h"
#include <string>
#include "Python.h"
namespace sl{
namespace script{
class PyScript : public sl::CSingleton<PyScript>{
public:
	PyScript():_module(NULL), _extraModule(NULL){}
	virtual ~PyScript(){}

	//安装和卸载脚本模块
	virtual bool install(const char* pythonHomeDir, const char* pyPaths, const char* moduleName);

	inline PyObject* getModule() const { return _module; }

protected:
	PyObject*			_module;
	PyObject*			_extraModule;		//扩展脚本模块

};
}
}

#endif
