#ifndef SL_CONFIG_ENGINE_H
#define SL_CONFIG_ENGINE_H
#include "sliconfig_engine.h"
#include <vector>
#include "slsingleton.h"
namespace sl{
namespace core{

class ConfigEngine : public SLSingleton<ConfigEngine>{
	friend class SLSingleton<ConfigEngine>;
public:
	virtual bool ready();
	virtual bool initialize();
	virtual bool destory();

	const sModuleConfig* getModuleConfig();
	const sCoreConfig* getCoreConfig();

	bool loadCoreConfig(const char* path);
	bool loadModuleConfig(const char* path);

private:
	ConfigEngine(){}
	~ConfigEngine(){}

private:
	sModuleConfig	m_stModuleConfig;
	sCoreConfig		m_stCoreConfig;
};
}
}

#endif
