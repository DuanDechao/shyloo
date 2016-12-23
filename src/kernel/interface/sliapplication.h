#ifndef KERNEL_IAPPLICATION_H
#define KERNEL_IAPPLICATION_H
#include "slmulti_sys.h"
namespace sl
{
namespace core
{
class IApplication
{
public:
	virtual ~IApplication() {}

	virtual bool ready() = 0;
	virtual bool initialize(int32 argc, char ** argv) = 0;
	virtual bool destory() = 0;
};
}
}

#endif