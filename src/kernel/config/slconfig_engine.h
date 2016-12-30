#ifndef SL_CONFIG_ENGINE_H
#define SL_CONFIG_ENGINE_H
#include "sliconfig_engine.h"
#include <vector>
namespace sl
{
namespace core
{
struct sModuleConfig{
	std::string strModulePath;
	std::vector<std::string> vctModules;
};
class ConfigEngine : public IConfigEngine
{
private:
	ConfigEngine(){}
	~ConfigEngine(){}

public:
	static IConfigEngine* getInstance();

	virtual bool ready();
	virtual bool initialize();
	virtual bool destory();

	virtual const char* getEnvirPath() { return nullptr; }
	virtual const char* getCoreFile() { return m_coreFile.c_str(); }
	virtual const char* getConfigFile() { return m_moduleFile.c_str(); }
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