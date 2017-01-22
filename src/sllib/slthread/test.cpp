#include "slmulti_sys.h"
#include "slthread.h"
using namespace sl::thread;
class TPTask : public ITPTask{
public:
	TPTask() :_loopCnt(0){}
	virtual bool SLAPI start(){ return true; }
	virtual bool SLAPI process(){
		Sleep(3000);
		return true;
	}
	virtual bool SLAPI end() { return true; }
	virtual TPTaskState SLAPI presentMainThread(){
		++_loopCnt;
		if (_loopCnt < 20)
			return TPTASK_STATE_CONTINUE_CHILDTHREAD;
		else
			return TPTASK_STATE_COMPLETED;
	}
private:
	int32 _loopCnt;
};
int main(){
	const int32 defaultThreadCnt = 3;
	const int32 maxThreadCnt = 20;
	const int32 newThreadCnt = 2;
	ISLThreadPool* threadPool = createThreadPool(newThreadCnt, defaultThreadCnt, maxThreadCnt);
	threadPool->start();

	int32 i = 0;
	while (true){
		Sleep(300);
		if (i < 30){
			TPTask* ptask = NEW TPTask();
			threadPool->addTask(ptask);
		}
		threadPool->onMainThreadTick();
		ECHO_TRACE("totalThreadCnt:%d, freeThreadCnt:%d, buffTaskCnt:%d, finiTaskCnt:%d",
			threadPool->currentThreadCount(), threadPool->currentFreeThreadCount(), threadPool->bufferTaskSize(), threadPool->finiTaskSize());
		i++;
	}
	return true;
}