#include "slasync_thread.h"
#include "sltime.h"
#include "slasync_base.h"
namespace sl
{
namespace core
{
#define ASYNC_QUEUE_SIZE 8196

AsyncThread::AsyncThread()
	:_runningQueue(ASYNC_QUEUE_SIZE),
	_completeQueue(ASYNC_QUEUE_SIZE),
	_terminate(false)
{}

void AsyncThread::start(){
	_thread = std::thread(&AsyncThread::threadProc, this);
}

void AsyncThread::terminate(){
	_terminate = true;
	_thread.join();
}

void AsyncThread::add(AsyncBase* base){
	_runningQueue.push(base);
}

void AsyncThread::loop(int64 overTime){
	int64 tick = sl::getTimeMilliSecond();
	AsyncBase* base = nullptr;
	while (_completeQueue.read(base)){
		base->onComplete();
		base->release();

		if ((int64)sl::getTimeMilliSecond() - tick >= overTime)
			break;
	}
}

void AsyncThread::threadProc(){
	while (!_terminate){
		AsyncBase* base = nullptr;
		if (_runningQueue.read(base)){
			base->onExecute();

			_completeQueue.push(base);
		}
		else{
			CSLEEP(1);
		}
	}
}

}
}