#ifndef SL_KERNEL_ITIMER_ENGINE_H
#define SL_KERNEL_ITIMER_ENGINE_H
#include "slicore.h"
#include "slikernel.h"
namespace sl
{
namespace core
{
class ITimerEngine: public ICore
{
public:
	virtual bool startTimer(api::ITimer* pTimer, int64 delay, int32 count, int64 interval) = 0;
	virtual bool killTimer(api::ITimer* pTimer) = 0;
	virtual bool pauseTimer(api::ITimer* pTimer) = 0;
	virtual bool resumeTimer(api::ITimer* pTimer) = 0;
	virtual int64 loop(int64 overTime) = 0;
};
}
}
#endif