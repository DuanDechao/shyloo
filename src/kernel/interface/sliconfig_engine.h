#ifndef SL_KERNEL_ICONFIG_ENGINE_H
#define SL_KERNEL_ICONFIG_ENGINE_H
#include "slicore.h"
#include "slikernel.h"
#include <string>
#include <vector>
namespace sl
{
namespace core
{
struct sCoreConfig{
	//server logic main loop frame tick
	int32 gameUpdateTick;
	int32 maxAsyncThreadNum;
	int32 channelReadBufferSize;
	int32 channelWriteBufferSize;
	//log
	std::string logPath;
	std::string logFile;
	std::string logFormat;
};

struct sModuleConfig{
	std::string strModulePath;
	std::vector<std::string> vctModules;
};

class IConfigEngine: public ICore
{
public:
	virtual const sModuleConfig* getModuleConfig() = 0;
	virtual const sCoreConfig* getCoreConfig() = 0;

	virtual const char* getEnvirPath() = 0;
	virtual const char* getCoreFile() = 0;
	virtual const char* getConfigFile() = 0;
    virtual const char* getScriptResPath() = 0;
};
}
}
#endif
