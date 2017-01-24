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
	int32 sLoopduration;

	//socket module
	int32 sNetlooptick;		//net logic loop frame tick

	//timer module
	int32 sTimerlooptick;

	//db module
	int32 sDBlooptick;

	//open db server?
	bool sOpenDBSvr;

	//db thread min num
	int32 sDBMinWorkThreadNum;

	//db thread max num
	int32 sDBMaxWorkThreadNum;

	//db connection num
	int32 sDBConnetionNum;
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
};
}
}
#endif