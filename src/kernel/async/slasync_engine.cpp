#include "slasync_engine.h"
#include "slconfig_engine.h"
#include "slasync_thread.h"
#include "slasync_base.h"
#include "sltime.h"
namespace sl
{
namespace core
{
bool AsyncEngine::ready(){
	return true;
}

bool AsyncEngine::initialize(){
	const sCoreConfig* conf = ConfigEngine::getInstance()->getCoreConfig();
	int32 threadCount = conf->maxAsyncThreadNum;
	_ths.resize(threadCount);
	for (int i = 0; i < threadCount; i++){
		_ths[i] = std::thread([&_workQueue]{
			_workQueue.run();
		});
	}

	return true;
}

bool AsyncEngine::destory(){
	_workQueue.stop();

	_threads.clear();

	DEL this;
	return true;
}

int64 AsyncEngine::loop(int64 overTime){
	int64 tick = sl::getTimeMilliSecond();
	if (_threads.size() <= 0)
		return 0;

	int64 perOverTime = overTime / _threads.size();
	for (AsyncThread* t : _threads){
		t->loop(perOverTime);
	}
	//ECHO_ERROR("AsyncEngine loop");
	return sl::getTimeMilliSecond() - tick;
}

void AsyncEngine::start(const int64 threadId, api::IAsyncHandler* handler, const char* debug){
	SLASSERT(!handler->getBase(), "wtf");
	SLASSERT(_threads.size() > 0, "wtf");
	AsyncBase* base = NEW AsyncBase(handler, debug);
	_threads[threadId % _threads.size()]->add(base);
}

void AsyncEngine::stop(api::IAsyncHandler* handler){
	SLASSERT(handler->getBase(), "wtf");
	((AsyncBase*)handler->getBase())->setInVaild();
}



}
}
