#ifndef KERNEL_ICORE_H
#define KERNEL_ICORE_H
namespace sl
{
namespace core
{
class ICore
{
public:
	virtual ~ICore() {}
	virtual bool ready() = 0;
	virtual bool initialize() = 0;
	virtual bool destory() = 0;
};
}
}

#endif