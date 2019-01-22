#ifndef SL_SCRIPTSTDOUTERR_H
#define SL_SCRIPTSTDOUTERR_H

#include "scriptobject.h"
#include "scriptstdout.h"
#include "scriptstderr.h"

namespace sl{ namespace pyscript{
class ScriptStdOutErr
{					
public:	
	ScriptStdOutErr();
	virtual ~ScriptStdOutErr();

	/** 
		安装和卸载这个模块 
	*/
	bool install(void);
	bool uninstall(void);
	bool isInstall(void) const{ return isInstall_; }

	virtual void error_msg(const wchar_t* msg, uint32 msglen);
	virtual void info_msg(const wchar_t* msg, uint32 msglen);

	void pyPrint(const std::string& str);

	inline std::wstring& buffer(){return sbuffer_;}

protected:
	ScriptStdErr* pStderr_;
	ScriptStdOut* pStdout_;
	PyObject* pyPrint_;
	bool isInstall_;
	std::wstring sbuffer_;
} ;

}
}

#endif // KBE_SCRIPTSTDOUTERR_H
