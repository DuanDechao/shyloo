#ifndef SL_KERNEL_ILOGIC_ENGINE_H
#define SL_KERNEL_ILOGIC_ENGINE_H
#include "slicore.h"
#include "slimodule.h"
namespace sl
{
namespace core
{
class ILogicEngine: public ICore
{
public:
	virtual api::IModule* findModule(const char* pModuleName) = 0;
};
}
}
#endif