#include "sllogic_engine.h"
#include "slimodule.h"
#include "slkernel.h"
#include "slconfig_engine.h"
#include "sltools.h"

#ifdef SL_OS_WINDOWS
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

namespace sl{
namespace core{
using namespace api;
bool LogicEngine::ready(){
	return true;
}

bool LogicEngine::initialize(){
	const sModuleConfig* pModuleConfig = ConfigEngine::getInstance()->getModuleConfig();
	std::vector<std::string>::const_iterator itor = pModuleConfig->vctModules.begin();
	std::vector<std::string>::const_iterator iend = pModuleConfig->vctModules.end();

	while(itor != iend){
		char path[1024] = {0};

#ifdef SL_OS_LINUX
		SafeSprintf(path, sizeof(path), "%s/%slib%s.so", sl::getAppPath(), pModuleConfig->strModulePath.c_str(), (*itor).c_str());
		//»ñÈ¡dllÂ·¾¶
		void* handle = dlopen(path, RTLD_LAZY);
		SLASSERT(handle, "dlopen so %s failed", path);

		GetModuleFun fun = (GetModuleFun) dlsym(handle, "GetLogicModule");
		SLASSERT(fun, "get function:GetLogicModule error");
#endif

#ifdef SL_OS_WINDOWS
		SafeSprintf(path, sizeof(path), "%s/%s/%s.dll", sl::getAppPath(), pModuleConfig->strModulePath.c_str(), (*itor).c_str());
		HINSTANCE hinst = ::LoadLibrary(path);
		api::GetModuleFun fun = (api::GetModuleFun)::GetProcAddress(hinst, NAME_OF_GET_LOGIC_FUN);
		SLASSERT(fun, "get function:GetLogicModule error[%d]", GetLastError());
#endif // SL_OS_WINDOWS

		if(!fun){
			printf("canot get dll[%s]\n", path);
			return false;
		}

		api::IModule* plogic = fun();

		while (plogic){
			const char* pName = plogic->getName();
			std::map<std::string, api::IModule *>::iterator mitor = m_mapModule.find(pName);
			if(mitor != m_mapModule.end()){
				printf("can't find module[%s]\n", pName);
				return false;
			}

			m_vecModule.push_back(plogic);
			m_mapModule.insert(std::make_pair(pName, plogic));
			plogic = plogic->getNext();
		}

		++itor;
	}

	{
		std::vector<api::IModule *>::iterator vitor = m_vecModule.begin();
		std::vector<api::IModule *>::iterator viend = m_vecModule.end();
		while(vitor != viend){
			KERNEL_LOG("initializing name %s.", (*vitor)->getName());
			bool res = (*vitor)->initialize(core::Kernel::getInstance());
			if(!res){
				printf("initialize name %s failed.\n", (*vitor)->getName());
				KERNEL_ERROR("initialize name %s failed.", (*vitor)->getName());
				return false;
			}
			++vitor;
		}

		vitor = m_vecModule.begin();
		while(vitor != viend){
			bool res = (*vitor)->launched(Kernel::getInstance());
			KERNEL_LOG("launching name %s.", (*vitor)->getName());
			if(!res){
				printf("launch name %s failed.\n", (*vitor)->getName());
				KERNEL_ERROR("launch name %s failed.", (*vitor)->getName());
				return false;
			}
			++vitor;
		}
	}

	return true;
}

bool LogicEngine::destory(){
	DEL this;
	return true;
}

LogicEngine::~LogicEngine(){
	std::map<std::string, api::IModule *>::iterator itor = m_mapModule.begin();
	std::map<std::string, api::IModule *>::iterator iend = m_mapModule.end();
	while(itor != iend){
		if(NULL != itor->second){
			itor->second->destory(Kernel::getInstance());
			itor->second = NULL;
		}
		++itor;
	}

	m_mapModule.clear();

	std::vector<api::IModule *>::iterator vitor = m_vecModule.begin();
	std::vector<api::IModule *>::iterator viend = m_vecModule.end();

	while(vitor != viend){
		if(*vitor != NULL){

		}
		++vitor;
	}
	m_vecModule.clear();

}

api::IModule* LogicEngine::findModule(const char* pModuleName){
	std::map<std::string, api::IModule*>::iterator itor = m_mapModule.find(pModuleName);
	if(itor == m_mapModule.end() || NULL == itor->second){
		return NULL;
	}

	return itor->second;
}

}
}
