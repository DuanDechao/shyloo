#ifndef SL_KERNEL_ICONFIG_ENGINE_H
#define SL_KERNEL_ICONFIG_ENGINE_H
#include "slicore.h"
#include "slikernel.h"
namespace sl
{
namespace core
{
class IConfigEngine: public ICore
{
public:
	virtual const char* getEnvirPath() = 0;
	virtual const char* getCoreFile() = 0;
	virtual const char* getConfigFile() = 0;
};
}
}
#endif