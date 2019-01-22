#ifndef SL_SCRIPTSTDOUT_H
#define SL_SCRIPTSTDOUT_H

#include "scriptobject.h"
namespace sl{ namespace pyscript{

class ScriptStdOutErr;
class ScriptStdOut: public ScriptObject
{
	/** 子类化 将一些py操作填充进派生类 */
	INSTANCE_SCRIPT_HREADER(ScriptStdOut, ScriptObject)
public:	
	ScriptStdOut(ScriptStdOutErr* pScriptStdOutErr);
	virtual ~ScriptStdOut();

	/** 
		python执行写操作 
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
