#ifndef SL_SCRIPTSTDOUT_H
#define SL_SCRIPTSTDOUT_H

#include "scriptobject.h"
namespace sl{ namespace pyscript{

class ScriptStdOutErr;
class ScriptStdOut: public ScriptObject
{
	/** ���໯ ��һЩpy�������������� */
	INSTANCE_SCRIPT_HREADER(ScriptStdOut, ScriptObject)
public:	
	ScriptStdOut(ScriptStdOutErr* pScriptStdOutErr);
	virtual ~ScriptStdOut();

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

#endif // KBE_SCRIPTSTDOUT_H
