#include "sllogic_engine.h"
#include <Windows.h>
#include "slimodule.h"
#include "slkernel.h"
namespace sl
{
namespace core
{
#ifdef SL_OS_LINUX
#include <dlfcn.h>
#endif // SL_OS_LINUX

LogicEngine::LogicEngine()
{

}
bool LogicEngine::ready()
{
	return true;
}

bool LogicEngine::initialize()
{
	std::vector<std::string> vecModuleFromConfig;
	std::vector<std::string>::const_iterator itor = vecModuleFromConfig.begin();
	std::vector<std::string>::const_iterator iend = vecModuleFromConfig.end();

	while(itor != iend){
		char path[512] = {0};

#ifdef SL_OS_LINUX
		//»ñÈ¡dllÂ·¾¶
		void* handle = dlopen(path, RTLD_LAZY);

		GetModuleFun fun = (GetModuleFun) dlsym(handle, "GetLogicModule");
#endif

#ifdef SL_OS_WINDOWS
		HINSTANCE hinst = ::LoadLibrary(path);
		api::GetModuleFun fun = (api::GetModuleFun)::GetProcAddress(hinst, NAME_OF_GET_LOGIC_FUN);
#endif // SL_OS_WINDOWS

		if(!fun){
			return false;
		}

		api::IModule* plogic = fun();

		while (plogic){
			const char* pName = plogic->getName();
			std::map<std::string, api::IModule *>::iterator mitor = m_mapModule.find(pName);
			if(mitor != m_mapModule.end())
			{
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
			bool res = (*vitor)->initialize(core::Kernel::getSingletonPtr());
			if(!res){
				return false;
			}
			++vitor;
		}

		vitor = m_vecModule.begin();
		while(vitor != viend){
			bool res = (*vitor)->launched(Kernel::getSingletonPtr());
			if(!res){
				return false;
			}
			++vitor;
		}
	}

	return true;
}

bool LogicEngine::destory()
{
	delete this;
	return true;
}

LogicEngine::~LogicEngine()
{
	std::map<std::string, api::IModule *>::iterator itor = m_mapModule.begin();
	std::map<std::string, api::IModule *>::iterator iend = m_mapModule.end();
	while(itor != iend){
		if(NULL != itor->second){
			itor->second->destory(Kernel::getSingletonPtr());
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

api::IModule* LogicEngine::findModule(const char* pModuleName)
{
	std::map<std::string, api::IModule*>::iterator itor = m_mapModule.find(pModuleName);
	if(itor == m_mapModule.end() || NULL == itor->second){
		return NULL;
	}

	return itor->second;
}

}
}