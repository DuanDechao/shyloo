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
		if(!handle){
            KERNEL_FATAL("dlopen so %s failed, error[%s]", path, dlerror());
			return false;
		}

		GetModuleFun fun = (GetModuleFun) dlsym(handle, "GetLogicModule");
		if(!fun){
			KERNEL_ERROR("get function:GetLogicModule error[%s]", (*itor).c_str());
			return false;
		}
#endif

#ifdef SL_OS_WINDOWS
		SafeSprintf(path, sizeof(path), "%s/%s/%s.dll", sl::getAppPath(), pModuleConfig->strModulePath.c_str(), (*itor).c_str());
		HINSTANCE hinst = ::LoadLibrary(path);
		api::GetModuleFun fun = (api::GetModuleFun)::GetProcAddress(hinst, NAME_OF_GET_LOGIC_FUN);
		if(!fun){
			KERNEL_ERROR(fun, "get function:GetLogicModule error[%s]", (*itor).c_str());
			return false;
		}
#endif // SL_OS_WINDOWS

		if(!fun){
			KERNEL_FATAL("canot get dll[%s], error[%d]", path, SL_ERRNO);
			return false;
		}

		api::IModule* plogic = fun();

		while (plogic){
			const char* pName = plogic->getName();
			std::map<std::string, api::IModule *>::iterator mitor = m_mapModule.find(pName);
			if(mitor != m_mapModule.end()){
				KERNEL_ERROR("can't find module[%s]", pName);
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
			KERNEL_INFO("initializing module %s.", (*vitor)->getName());
			bool res = (*vitor)->initialize(core::Kernel::getInstance());
			if(!res){
				KERNEL_ERROR("initialize module %s failed.", (*vitor)->getName());
				return false;
			}
			++vitor;
		}

		vitor = m_vecModule.begin();
		while(vitor != viend){
			bool res = (*vitor)->launched(Kernel::getInstance());
			KERNEL_INFO("launching module %s.", (*vitor)->getName());
			if(!res){
				KERNEL_ERROR("launch module %s failed.", (*vitor)->getName());
				return false;
			}
			(*vitor)->setInited(true);
			++vitor;
		}
	}

	return true;
}

bool LogicEngine::destory(){
	std::map<std::string, api::IModule *>::iterator itor = m_mapModule.begin();
	std::map<std::string, api::IModule *>::iterator iend = m_mapModule.end();
	while(itor != iend){
		if(NULL != itor->second && itor->second->isInited()){
			KERNEL_INFO("destroy module %s", itor->second->getName());
			itor->second->destory(Kernel::getInstance());
			itor->second = NULL;
		}
		++itor;
	}

	m_mapModule.clear();
	m_vecModule.clear();

	DEL this;
	return true;
}

LogicEngine::~LogicEngine(){
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
