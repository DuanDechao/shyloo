#include "sltpthread.h"
#include "slthread_pool.h"
#include <process.h>

HANDLE TPThread::createThread(){
#ifdef SL_OS_WINDOWS
	m_threadId = (HANDLE)_beginthreadex(NULL, 0, &TPThread::threadFunc, (void*)this, NULL, 0);
#else
	if(pthread_create(&m_threadId, NULL, TPThread::threadFunc, (void*)this) != 0){
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

TPTask* TPThread::tryGetTask(){
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