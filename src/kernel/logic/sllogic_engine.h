#ifndef SL_KERNEL_LOGIC_ENGINE_H
#define SL_KERNEL_LOGIC_ENGINE_H
#include <vector>
#include <map>
#include "slilogic_engine.h"
#include "slimodule.h"
#include "slsingleton.h"
namespace sl
{
namespace core
{
class LogicEngine: public SLSingleton<LogicEngine>{
	friend class SLSingleton<LogicEngine>;
public:
	virtual bool ready();
	virtual bool initialize();
	virtual bool destory();
	virtual api::IModule* findModule(const char* pModuleName);

private:
	LogicEngine(){}
	~LogicEngine();

private:
	std::vector<api::IModule *> m_vecModule;
	std::map<std::string, api::IModule *> m_mapModule;
};
}
}
#endif