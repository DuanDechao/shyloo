#ifndef SL_TIMER_ENGINE_H
#define SL_TIMER_ENGINE_H
#include "sliconfig_engine.h"
#include "slsingleton.h"
#include <vector>
namespace sl
{
namespace core
{
struct sModuleConfig{
	std::string strModulePath;
	std::vector<std::string> vctModules;
};
class ConfigEngine : public IConfigEngine, public CSingleton<ConfigEngine>
{
private:
	ConfigEngine();
	~ConfigEngine();

public:
	virtual bool ready();
	virtual bool initialize();
	virtual bool destory();

	const char* getCoreFile() const { return m_coreFile.c_str(); }
	const char* getModuleFile() const { return m_moduleFile.c_str(); }
private:
	bool loadModuleConfig();
	bool loadCoreConfig();

private:
	sModuleConfig	m_stModuleConfig;
	std::string		m_coreFile;
	std::string		m_moduleFile;
};
}
}

#endif