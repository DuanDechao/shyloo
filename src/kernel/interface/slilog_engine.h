#ifndef KERNEL_ILOG_ENGINE_H
#define KERNEL_ILOG_ENGINE_H
#include "slicore.h"
#include "slikernel.h"
namespace sl
{
namespace core
{
class ILogEngine: public ICore{
public:
	virtual void logSync(int32 filter, const char* log, const char* file, const int32 line) = 0;
	virtual void logAsync(int32 filter, const char* log, const char* file, const int32 line) = 0;
	virtual int64 loop(int64 overTime) = 0;
};

}
}

#endif