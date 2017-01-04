#ifndef SL_KERNEL_ICONFIG_ENGINE_H
#define SL_KERNEL_ICONFIG_ENGINE_H
#include "slicore.h"
#include "slikernel.h"
namespace sl
{
namespace core
{
struct sCoreConfig{

};

struct sModuleConfig{
	std::string strModulePath;
	std::vector<std::string> vctModules;
};

class IConfigEngine: public ICore
{
public:
	virtual const sModuleConfig* getModuleConfig() = 0;

	virtual const char* getEnvirPath() = 0;
	virtual const char* getCoreFile() = 0;
	virtual const char* getConfigFile() = 0;
};
}
}
#endif