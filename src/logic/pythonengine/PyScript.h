#ifndef SL_PY_SCRIPT_SCRIPT_H
#define SL_PY_SCRIPT_SCRIPT_H
#include <string>
#include "Python.h"
class PyScript{
public:
	PyScript():_baseModule(NULL), _mathModule(NULL), _extraModule(NULL){}
	virtual ~PyScript(){}

	//��װ��ж�ؽű�ģ��
	virtual bool install(const char* pythonHomeDir, const char* pyPaths, const char* moduleName);
	virtual bool uninstall();

	inline PyObject* getBaseModule() const { return _baseModule; }
	inline PyObject* getMathModule() const { return _mathModule; }
	
	bool runSimpleString(const char* command, std::string* retPtr);

protected:
	PyObject*			_baseModule;
	PyObject*			_mathModule;
	PyObject*			_extraModule;		//��չ�ű�ģ��

};
#endif
