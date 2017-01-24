#include "slthread_pool.h"
#include "sltpthread.h"
#include "sltime.h"
#include <iterator>
namespace sl{
namespace thread
{
SLThreadPool::SLThreadPool(int32 newThreadCount, int32 defaultThreadCount, int32 maxThreadCount)
	:m_maxThreadCount(maxThreadCount),
	m_defaultThreadCount(defaultThreadCount),
	m_extraNewAddThreadCount(newThreadCount),
	m_isInitialize(false),
	m_isDestroyed(false),
	m_bufferedTaskList(),
	m_finiTaskList(),
	m_finiTaskListCount(0),
	m_busyThreadList(),
	m_freeThreadList(),
	m_allThreadList(),
	m_currThreadCount(0),
	m_currFreeThreadCount(0)
{
	InitializeCriticalSection(&m_finiTaskListMutex);
	InitializeCriticalSection(&m_bufferedTaskListMutex);
	InitializeCriticalSection(&m_threadStateListMutex);
}

SLThreadPool::~SLThreadPool(){
	SLASSERT(m_isDestroyed && m_allThreadList.size() == 0, "wtf");
}

void SLThreadPool::release(){
	destroy();
	DEL this;
}

void SLThreadPool::destroy(){
	m_isDestroyed = true;

	int32 count = m_allThreadList.size();

	if (count > 0){
		while (true){
			Sleep(300);

			EnterCriticalSection(&m_threadStateListMutex);

			auto itor = m_allThreadList.begin();
			for (; itor != m_allThreadList.end(); ++itor){
				if ((*itor)->state() != TPThread::THREAD_STATE_END){
					(*itor)->sendCondSignal();
				}
				else{
					--count;
				}
			}

			LeaveCriticalSection(&m_threadStateListMutex);

			if (count <= 0)
				break;
		}

		EnterCriticalSection(&m_threadStateListMutex);
		auto itor1 = m_allThreadList.begin();
		for (; itor1 != m_allThreadList.end(); ++itor1){
			if ((*itor1)){
				DEL(*itor1);
				*itor1 = NULL;
			}
		}
		m_allThreadList.clear();
		m_currFreeThreadCount = 0;
		m_currThreadCount = 0;
		LeaveCriticalSection(&m_threadStateListMutex);
	}

	if (m_finiTaskList.size() > 0){
		EnterCriticalSection(&m_finiTaskListMutex);
		auto itor2 = m_finiTaskList.begin();
		for (; itor2 != m_finiTaskList.end(); ++itor2){
			if ((*itor2)){
				(*itor2)->release();
				*itor2 = NULL;
			}
		}
		m_finiTaskList.clear();
		m_finiTaskListCount = 0;
		LeaveCriticalSection(&m_finiTaskListMutex);
	}
	
	if (m_bufferedTaskList.size() > 0){
		EnterCriticalSection(&m_bufferedTaskListMutex);
		while (!m_bufferedTaskList.empty()){
			ITPTask* pTask = m_bufferedTaskList.front();
			m_bufferedTaskList.pop();
			if (pTask)
				pTask->release();
		}
		LeaveCriticalSection(&m_bufferedTaskListMutex);
	}

	DeleteCriticalSection(&m_bufferedTaskListMutex);
	DeleteCriticalSection(&m_threadStateListMutex);
	DeleteCriticalSection(&m_finiTaskListMutex);
}

bool SLThreadPool::start(){
	SLASSERT(!m_isInitialize, "wtf");

	for (int32 i = 0; i < m_defaultThreadCount; i++){
		TPThread* tptd = createThread(0);
		if (!tptd){
			ECHO_ERROR("create TPThread failed!");
			return false;
		}
		m_currFreeThreadCount++;
		m_currThreadCount++;

		m_freeThreadList.push_back(tptd);
		m_allThreadList.push_back(tptd);
	}

	m_isInitialize = true;
	return true;
}

bool SLThreadPool::run(int64 overTime){
	int64 startTime = sl::getTimeMilliSecond();
	while (mainThreadTick()){
		if (sl::getTimeMilliSecond() - startTime >= overTime)
			break;
	}
	return true;
}

bool SLThreadPool::mainThreadTick(){
	std::vector<ITPTask*> finiTasks;
	
	EnterCriticalSection(&m_finiTaskListMutex);
	if (m_finiTaskList.size() == 0){
		LeaveCriticalSection(&m_finiTaskListMutex);
		return false;
	}

	std::copy(m_finiTaskList.begin(), m_finiTaskList.end(), std::back_inserter(finiTasks));
	m_finiTaskList.clear();
	LeaveCriticalSection(&m_finiTaskListMutex);

	if (finiTasks.empty())
		return false;

	auto finiIter = finiTasks.begin();
	for (; finiIter != finiTasks.end();){
		TPTaskState state = (*finiIter)->presentMainThread();

		switch (state)
		{
		case sl::thread::TPTASK_STATE_COMPLETED:
			(*finiIter)->release();
			finiIter = finiTasks.erase(finiIter);
			--m_finiTaskListCount;
			break;

		case sl::thread::TPTASK_STATE_CONTINUE_MAINTHREAD:
			EnterCriticalSection(&m_finiTaskListMutex);
			m_finiTaskList.push_back(*finiIter);
			LeaveCriticalSection(&m_finiTaskListMutex);
			++finiIter;
			break;

		case sl::thread::TPTASK_STATE_CONTINUE_CHILDTHREAD:
			this->addTask(*finiIter);
			finiIter = finiTasks.erase(finiIter);
			--m_finiTaskListCount;
			break;

		default:
			SLASSERT(false, "invaild task state");
			break;
		}
		
	}
	return true;
}

TPThread* SLThreadPool::createThread(int32 threadWaitSecond){
	TPThread* tptd = NEW TPThread(this, threadWaitSecond);
	tptd->createThread();
	return tptd;
}

void SLThreadPool::bufferTask(ITPTask* pTask){
	EnterCriticalSection(&m_bufferedTaskListMutex);
	m_bufferedTaskList.push(pTask);

	size_t size = m_bufferedTaskList.size();

	if (size > THREAD_BUSY_SIZE){
		//warning
	}
	LeaveCriticalSection(&m_bufferedTaskListMutex);
}

bool SLThreadPool::addTask(ITPTask* pTask){

	EnterCriticalSection(&m_threadStateListMutex);
	
	if (m_currFreeThreadCount > 0){
		auto itor = m_freeThreadList.begin();
		TPThread* pThread = (TPThread*)(*itor);
		m_freeThreadList.erase(itor);
		m_busyThreadList.push_back(pThread);
		--m_currFreeThreadCount;

		pThread->setTask(pTask);

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

	bufferTask(pTask);

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

void SLThreadPool::addFiniTask(ITPTask* pTask){
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

ITPTask* SLThreadPool::popBufferTask(){
	ITPTask* pTask = NULL;
	EnterCriticalSection(&m_bufferedTaskListMutex);

	size_t size = m_bufferedTaskList.size();
	if (size > 0){
		pTask = m_bufferedTaskList.front();
		m_bufferedTaskList.pop();

		if (size > THREAD_BUSY_SIZE){
			//warning
		}
	}

	LeaveCriticalSection(&m_bufferedTaskListMutex);
	return pTask;
}

bool SLThreadPool::hasThread(TPThread* td){
	bool ishas = true;

	EnterCriticalSection(&m_threadStateListMutex);

	auto itor = std::find(m_allThreadList.begin(), m_allThreadList.end(), td);
	if (itor == m_allThreadList.end())
		ishas = false;

	LeaveCriticalSection(&m_threadStateListMutex);

	return ishas;
}

ISLThreadPool* SLAPI createThreadPool(int32 newThreadCount, int32 defaultThreadCount, int32 maxThreadCount){
	return NEW SLThreadPool(newThreadCount, defaultThreadCount, maxThreadCount);
}

}
}