#ifndef SL_IMODULE_H
#define SL_IMODULE_H
#include "slikernel.h"
#include "slstring_utils.h"
#include "sltime.h"
namespace sl{
namespace api{
#define MODULE_NAME_LENGTH 64
class IModule{
public:
	virtual ~IModule(){}
	virtual bool initialize(IKernel* pKernel) = 0;
	virtual bool launched(IKernel* pKernel) = 0;
	virtual bool destory(IKernel* pKernel) = 0;

public:
	IModule(){
		m_pNextModule = nullptr;
		m_pName[MODULE_NAME_LENGTH -1] = 0;
	}

	bool setNext(IModule * & pModule){
		m_pNextModule = pModule;
		return true;
	}

	IModule* getNext(){
		return m_pNextModule;
	}

	bool setName(const char * pName){
		safeMemcpy(m_pName, sizeof(m_pName), pName, MODULE_NAME_LENGTH -1);
		return true;
	}

	const char* getName() const{
		return m_pName;
	}


private:
	IModule*		m_pNextModule;
	char			m_pName[MODULE_NAME_LENGTH];
};

typedef IModule * (*GetModuleFun)(void);

#define NAME_OF_GET_LOGIC_FUN "GetLogicModule"
#define GET_LOGIC_FUN GetLogicModule


#define CREATE_MODULE(name) \
	class factory##name		\
	{      \
	public:	\
		factory##name(IModule* & pModule)	\
		{    \
			IModule * module##name = new name;    \
			module##name->setName(#name);    \
			module##name->setNext(pModule); \
			pModule = module##name;    \
		}	\
	};	\
	factory##name factory##name(plogicModule);

#ifdef SL_OS_WINDOWS
#define GET_DLL_ENTRANCE	\
	static IModule*	plogicModule = NULL; \
	extern "C" __declspec(dllexport) IModule* __cdecl GET_LOGIC_FUN() {   \
		return plogicModule;	\
	}\
	BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) { \
		return true;	\
	}
#endif // SL_OS_WINDOWS

#ifdef SL_OS_LINUX
#define GET_DLL_ENTRANCE	\
	static IModule * plogicModule = NULL;	\
	extern "C" IModule * GET_LOGIC_FUN() { \
	srand(sl::getTimeMilliSecond());	\
	return plogicModule;	\
}
#endif // SL_OS_LINUX

}
}

#endif
