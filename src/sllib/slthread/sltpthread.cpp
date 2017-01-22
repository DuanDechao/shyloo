#include "sltpthread.h"
#include "slthread_pool.h"
#include <process.h>
namespace sl
{
namespace thread
{
HANDLE TPThread::createThread(){
#ifdef SL_OS_WINDOWS
	m_threadId = (HANDLE)_beginthreadex(NULL, 0, &TPThread::threadFunc, (void*)this, NULL, 0);
#else
	if (pthread_create(&m_threadId, NULL, TPThread::threadFunc, (void*)this) != 0){
		SLASSERT(false, "create thread failed");
	}
#endif
	return m_threadId;
}

void TPThread::onTaskCompleted(){
	m_threadPool->addFiniTask(m_currTask);
	m_currTask = NULL;
	m_threadPool->addFreeThread(this);
}

ITPTask* TPThread::tryGetTask(){
	return m_threadPool->popBufferTask();
}

bool TPThread::onWaitCondSignal(){
#ifdef SL_OS_WINDOWS
	if (m_threadWaitSecond <= 0){
		m_state = THREAD_STATE_SLEEP;
		WaitForSingleObject(m_cond, INFINITE);
		ResetEvent(m_cond);
	}
	else{
		m_state = THREAD_STATE_SLEEP;
		DWORD ret = WaitForSingleObject(m_cond, m_threadWaitSecond * 1000);
		ResetEvent(m_cond);

		//如果是因为超时了，说明该线程空闲太久，可以注销
		if (ret == WAIT_TIMEOUT){
			m_threadPool->removeHangThread(this);
			return false;
		}
		else if (ret != WAIT_OBJECT_0){
			ECHO_ERROR("WaitForSingleObject error");
		}
	}
#else //liunx

#endif
	return true;
}

bool TPThread::join(){
#ifdef SL_OS_WINDOWS
	int32 i = 0;
	while (true){
		++i;
		DWORD dw = WaitForSingleObject(id(), 3000);

		switch (dw)
		{
		case WAIT_OBJECT_0:
			return true;
		case WAIT_TIMEOUT:
			if (i > 20){
				ECHO_ERROR("TPThread:join can't join thread %p", (void*)this);
				return false;
			}
			else{
				ECHO_TRACE("TPThread:join waiting for thread %p, try:%d", (void*)this, i);
			}
			break;
		case WAIT_FAILED:
		default:
			ECHO_ERROR("TPThread:join can't join thread %p", (void*)this);
			break;
		};
	}
#else //linux
#endif
	return true;
}

#ifdef SL_OS_WINDOWS
unsigned __stdcall TPThread::threadFunc(void *args){
#else
void* TPThread::threadFunc(void* args){
#endif

	TPThread* pThread = static_cast<TPThread*>(args);
	SLASSERT(pThread, "invaild thread pointer");
	SLThreadPool* pThreadPool = pThread->threadPool();
	SLASSERT(pThread, "thread[%p] have no threadpool pointer", (void*)pThread);

	pThread->resetDoneTasks();
#ifdef SL_OS_WINDOWS
#else
	pthread_detech(pthread_self());
#endif

	bool isRun = true;
	bool isDestroy = false;
	while (isRun){
		if (pThread->task() != NULL){
			isRun = true;
		}
		else{
			pThread->resetDoneTasks();
			isRun = pThread->onWaitCondSignal();
		}

		if (!isRun || pThreadPool->isDestroyed()){
			if (!pThreadPool->hasThread(pThread)){
				pThread = NULL;
			}
			isDestroy = true;
			break;
		}

		ITPTask* pTask = pThread->task();
		if (pTask == NULL)
			continue;

		pThread->setState(THREAD_STATE_BUSY);

		while (pTask && !pThreadPool->isDestroyed()){
			//执行任务
			pThread->incDoneTasks();
			pThread->onTPTaskStart();
			pThread->onTPTaskProcess();
			pThread->onTPTaskEnd();

			//执行完了尝试获取下一个任务
			ITPTask* pNewTask = pThread->tryGetTask();
			if (pNewTask == NULL){
				pThread->onTaskCompleted();
				break;
			}
			else{
				pThreadPool->addFiniTask(pTask);
				pThread->setTask(pNewTask);
				pTask = pNewTask;
			}
		}
	}

	if (isDestroy && pThread){
		ITPTask* pTask = pThread->task();
		if (pTask != NULL){
			DEL pTask;
			pThread->setTask(NULL);
		}
			
		pThread->onTPTaskEnd();
		pThread->setState(THREAD_STATE_END);
		pThread->resetDoneTasks();
	}

#ifdef SL_OS_WINDOWS
	return 0;
#else
	return (void*)NULL;
#endif

}

}
}
