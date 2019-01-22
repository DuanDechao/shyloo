#ifndef SL_SCRIPTSTDERR_H
#define SL_SCRIPTSTDERR_H

#include "scriptobject.h"
namespace sl{ namespace pyscript{

class ScriptStdOutErr;
class ScriptStdErr: public ScriptObject
{
	/** ���໯ ��һЩpy�������������� */
	INSTANCE_SCRIPT_HREADER(ScriptStdErr, ScriptObject)
public:	
	ScriptStdErr(ScriptStdOutErr* pScriptStdOutErr);
	virtual ~ScriptStdErr();

	/** 
		pythonִ��д���� 
	*/
	static PyObject* __py_write(PyObject* self, PyObject *args);
	static PyObject* __py_flush(PyObject* self, PyObject *args);

	bool install(void);
	bool uninstall(void);
	bool isInstall(void) const{ return isInstall_; }

	inline ScriptStdOutErr* pScriptStdOutErr() const {return pScriptStdOutErr_;}

protected:
	bool softspace_;
	PyObject* old_stdobj_;
	bool isInstall_;

	ScriptStdOutErr* pScriptStdOutErr_;
} ;

}
}

#endif // SL_SCRIPTSTDERR_H
