#ifndef SL_SCRIPTSTDOUTERRHOOK_H
#define SL_SCRIPTSTDOUTERRHOOK_H

#include "scriptobject.h"
#include "scriptstdouterr.h"
namespace sl{ namespace pyscript{

class ScriptStdOutErrHook : public ScriptStdOutErr
{
public:
	ScriptStdOutErrHook();
	~ScriptStdOutErrHook();

	virtual void error_msg(const wchar_t* msg, uint32 msglen);
	virtual void info_msg(const wchar_t* msg, uint32 msglen);

	inline void setHookBuffer(std::string* buffer){
		buffer_ = buffer; 
		wbuffer_ = L""; 
	}

	inline void setPrint(bool v){
		isPrint_ = v;
	}

protected:
	std::string* buffer_;
	std::wstring wbuffer_;
	bool isPrint_;
} ;

}
}

#endif // SL_SCRIPTSTDOUTERRHOOK_H
