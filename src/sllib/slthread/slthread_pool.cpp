#include "slthread_pool.h"
#include "sltpthread.h"

SLThreadPool::SLThreadPool(int32 newThreadCount, int32 defaultThreadCount, int32 maxThreadCount)
	:m_maxThreadCount(maxThreadCount),
	m_defaultThreadCount(defaultThreadCount),
	m_extraNewAddThreadCount(newThreadCount),
	m_isInitialize(false)
{
	for (int32 i = 0; i < defaultThreadCount; i++){
		TPThread* tptd = createThread(0);
		if (!tptd){
			ECHO_ERROR("create TPThread failed!");
			return;
		}
		m_currFreeThreadCount++;
		m_currThreadCount++;

		m_freeThreadList.push_back(tptd);
		m_allThreadList.push_back(tptd);
	}

	m_isInitialize = true;
}

void SLThreadPool::onMainThreadTick(){
	std::vector<TPTask*> finiTasks;
	
	EnterCriticalSection(&m_finiTaskListMutex);
	if (m_finiTaskList.size() == 0){
		LeaveCriticalSection(&m_finiTaskListMutex);
		return;
	}

	std::copy(m_finiTaskList.begin(), m_finiTaskList.end(), std::back_inserter(finiTasks));
	m_finiTaskList.clear();
	LeaveCriticalSection(&m_finiTaskListMutex);

	for (auto finiIter = finiTasks.begin(); finiIter != finiTasks.end(); ++finiIter){
		
	}
}

TPThread* SLThreadPool::createThread(int32 threadWaitSecond){
	TPThread* tptd = NEW TPThread(this, threadWaitSecond);
	tptd->createThread();
	return tptd;
}

void SLThreadPool::bufferTask(TPTask* tpTask){
	EnterCriticalSection(&m_bufferedTaskListMutex);
	m_bufferedTaskList.push(tpTask);

	size_t size = m_bufferedTaskList.size();

	if (size > THREAD_BUSY_SIZE){
		//warning
	}
	LeaveCriticalSection(&m_bufferedTaskListMutex);
}

bool SLThreadPool::addTask(TPTask* tpTask){
	EnterCriticalSection(&m_threadStateListMutex);
	if (m_currFreeThreadCount > 0){
		auto itor = m_freeThreadList.begin();
		TPThread* pThread = (TPThread*)(*itor);
		m_freeThreadList.erase(itor);
		m_busyThreadList.push_back(pThread);
		--m_currFreeThreadCount;

		pThread->setTask(tpTask);

#ifdef SL_OS_WINDOWS
		if (pThread->sendCondSignal() == 0){
#else
		if (pThread->sendCondSignal() != 0){
#endif
			ECHO_ERROR("add task cond signal error!");
			LeaveCriticalSection(&m_threadStateListMutex);
			return false;
		}
		
		LeaveCriticalSection(&m_threadStateListMutex);
		return true;
	}

	bufferTask(tpTask);

	if (isThreadCountMax()){
		LeaveCriticalSection(&m_threadStateListMutex);
		return false;
	}

	for (int32 i = 0; i < m_extraNewAddThreadCount; i++){
		//设定5分钟最大空闲时间
		TPThread* tptd = createThread(300);
		if (!tptd){
			ECHO_ERROR("threadpool create thread failed");
			continue;
		}

		//所有的线程列表
		m_allThreadList.push_back(tptd);

		//空闲的线程列表
		m_freeThreadList.push_back(tptd);

		++m_currThreadCount;
		++m_currFreeThreadCount;
	}

	LeaveCriticalSection(&m_threadStateListMutex);
	return true;
}

bool SLThreadPool::removeHangThread(TPThread* td){
	EnterCriticalSection(&m_threadStateListMutex);
	std::list<TPThread*>::iterator freeIter, allIter;
	freeIter = std::find(m_freeThreadList.begin(), m_freeThreadList.end(), td);
	allIter = std::find(m_allThreadList.begin(), m_allThreadList.end(), td);

	if (freeIter != m_freeThreadList.end() && allIter != m_allThreadList.end()){
		m_freeThreadList.erase(freeIter);
		m_allThreadList.erase(allIter);
		--m_currFreeThreadCount;
		--m_currThreadCount;
		SAFE_RELEASE(td);
	}
	else{
		LeaveCriticalSection(&m_threadStateListMutex);
		ECHO_ERROR("remove hang thread, but can't find thread[%d]", td->id());
		return false;
	}
	LeaveCriticalSection(&m_threadStateListMutex);
	return true;
}

void SLThreadPool::addFiniTask(TPTask* pTask){
	EnterCriticalSection(&m_finiTaskListMutex);
	m_finiTaskList.push_back(pTask);
	++m_finiTaskListCount;
	LeaveCriticalSection(&m_finiTaskListMutex);
}

bool SLThreadPool::addBusyThread(TPThread* td){
	EnterCriticalSection(&m_threadStateListMutex);
	std::list<TPThread*>::iterator itor = std::find(m_freeThreadList.begin(), m_freeThreadList.end(), td);

	if (itor != m_freeThreadList.end()){
		m_freeThreadList.erase(itor);
	}
	else{
		LeaveCriticalSection(&m_threadStateListMutex);
		ECHO_ERROR("add busy thread, but can't find thread[%d] in freeThreadList", td->id());
		DEL td;
		return false;
	}

	m_busyThreadList.push_back(td);
	--m_currFreeThreadCount;
	LeaveCriticalSection(&m_threadStateListMutex);

	return true;
}

bool SLThreadPool::addFreeThread(TPThread* td){
	EnterCriticalSection(&m_threadStateListMutex);
	std::list<TPThread*>::iterator itor = std::find(m_busyThreadList.begin(), m_busyThreadList.end(), td);

	if (itor != m_busyThreadList.end()){
		m_busyThreadList.erase(itor);
	}
	else{
		LeaveCriticalSection(&m_threadStateListMutex);
		ECHO_ERROR("add free thread, but can't find thread[%d] in busyThreadList", td->id());
		DEL td;
		return false;
	}

	m_freeThreadList.push_back(td);
	++m_currFreeThreadCount;
	LeaveCriticalSection(&m_threadStateListMutex);

	return true;
}

TPTask* SLThreadPool::popBufferTask(){
	TPTask* tpTask = NULL;
	EnterCriticalSection(&m_bufferedTaskListMutex);

	size_t size = m_bufferedTaskList.size();
	if (size > 0){
		tpTask = m_bufferedTaskList.front();
		m_bufferedTaskList.pop();

		if (size > THREAD_BUSY_SIZE){
			//warning
		}
	}

	LeaveCriticalSection(&m_bufferedTaskListMutex);
	return tpTask;
}