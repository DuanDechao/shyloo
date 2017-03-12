#ifndef __SL_ASYNC_ENGINE_H__
#define __SL_ASYNC_ENGINE_H__
#include "slikernel.h"
#include <vector>
#include "slsingleton.h"
namespace sl
{
namespace core
{

class AsyncThread;
class AsyncEngine: public SLSingleton<AsyncEngine>{
	friend class SLSingleton<AsyncEngine>;
public:
	virtual bool ready();
	virtual bool initialize();
	virtual bool destory();

	int64 loop(int64 overTime);

	void start(const int64 threadId, api::IAsyncHandler* handler, const char* debug);
	void stop(api::IAsyncHandler* handler);

private:
	std::vector<AsyncThread*> _threads;
};


}
}
#endif