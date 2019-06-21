#ifndef __SL_ASYNC_THREAD_H_
#define __SL_ASYNC_THREAD_H_

#include "slmulti_sys.h"
#include "slcycle_queue.h"
#include <thread>

namespace sl{
namespace core{

class AsyncBase;
class AsyncThread{
public:
	AsyncThread(SLStrandWorkQueue* workQueue);
	~AsyncThread(){}

	AsyncThread(const AsyncThread&) = delete;

	void start();
	void terminate();

	//void add(AsyncBase* base);

//	void loop(int64 overTime);
	void threadProc();

private:
	bool				_terminate;
	std::thread			_thread;
	SLStrandWorkQueue*	_workQueue;
};

}
}
#endif
