#include "slevent_dispatcher.h"
#include "slevent_poller.h"
namespace sl
{
namespace network
{
EventDispatcher::EventDispatcher()
	:m_breakProcessing(EVENT_DISPATCHER_STATUS_RUNNING),
	 m_maxWait(0.1),
	 m_numTimeCalls(0),
	 m_accSpareTime(0),
	 m_oldSpareTime(0),
	 m_totSpareTime(0),
	 m_lastStatisticsGathered(0),
	 m_pTasks(new Tasks),
	 m_pErrorReporter(NULL),
	 m_pTimers(new Timers64)
{
	m_pPoller = EventPoller::create();
	//m_pErrorReporter = new ErrorReporter(*this);
}

EventDispatcher::~EventDispatcher()
{
	SAFE_RELEASE(m_pErrorReporter);
	SAFE_RELEASE(m_pTasks);
	SAFE_RELEASE(m_pPoller);

	if(!m_pTimers->empty())
	{

	}

	m_pTimers->clear(false);
	SAFE_RELEASE(m_pTimers);
}

EventPoller* EventDispatcher::createPoller()
{
	m_pPoller = EventPoller::create();
	return m_pPoller;
}

bool EventDispatcher::registerReadFileDescriptor(int fd, InputNotificationHandler* handler)
{
	return m_pPoller->registerForRead(fd, handler);
}

bool EventDispatcher::registerWriteFileDescriptor(int fd, OutputNotificationHandler* handler)
{
	return m_pPoller->registerForWrite(fd, handler);
}

bool EventDispatcher::deregisterReadFileDescriptor(int fd)
{
	return m_pPoller->deregisterForRead(fd);
}

bool EventDispatcher::deregisterWriteFileDescriptor(int fd)
{
	return m_pPoller->deregisterForWrite(fd);
}

TimerHandle EventDispatcher::addTimerCommon(int64 microseconds, TimerHandler* handler, 
											void* arg, bool recurrent)
{
	SL_ASSERT(handler);

	if(microseconds <= 0)
		return TimerHandle();

	uint64 interval = int64( (((double)microseconds)/1000000.0) * stampsPerSecondD());

	TimerHandle handle = m_pTimers->add(timestamp() + interval,
		recurrent ? interval : 0,
		handler, arg);

	return handle;
}

uint64 EventDispatcher::getSpareTime() const
{
	return m_pPoller->spareTime();
}

void EventDispatcher::clearSpareTime()
{
	m_accSpareTime += m_pPoller->spareTime();
	m_pPoller->clearSpareTime();
}

void EventDispatcher::addTask(Task* pTask)
{
	m_pTasks->add(pTask);
}

bool EventDispatcher::cancelTask(Task* pTask)
{
	return m_pTasks->cancel(pTask);
}

double EventDispatcher::calculateWait() const
{
	double maxWait = m_maxWait;
	if(!m_pTimers->empty())
	{
		maxWait = min(maxWait, m_pTimers->nextExp(timestamp()) / stampsPerSecondD());
	}

	return maxWait;
}

void EventDispatcher::processTasks()
{
	m_pTasks->process();
}

void EventDispatcher::processTimers()
{
	m_numTimeCalls += m_pTimers->process(timestamp());
}

void EventDispatcher::processStats()
{
	if(timestamp() - m_lastStatisticsGathered >= stampsPerSecond())
	{
		m_oldSpareTime = m_totSpareTime;
		m_totSpareTime = m_accSpareTime + m_pPoller->spareTime();

		m_lastStatisticsGathered = timestamp();
	}
}

int EventDispatcher::processNetwork(bool shouldIdle)
{
	double maxWait = shouldIdle ? this->calculateWait() : 0.0;
	return m_pPoller->processPendingEvents(maxWait);
}

void EventDispatcher::processUntilBreak()
{
	if(m_breakProcessing != EVENT_DISPATCHER_STATUS_BREAK_PROCESSING)
		m_breakProcessing = EVENT_DISPATCHER_STATUS_RUNNING;

	while(m_breakProcessing != EVENT_DISPATCHER_STATUS_BREAK_PROCESSING)
	{
		this->processOnce(true);
	}
}

int EventDispatcher::processOnce(bool shouldIdle /* = false */)
{
	if(m_breakProcessing != EVENT_DISPATCHER_STATUS_BREAK_PROCESSING)
		m_breakProcessing = EVENT_DISPATCHER_STATUS_RUNNING;

	this->processTasks();

	if(m_breakProcessing != EVENT_DISPATCHER_STATUS_BREAK_PROCESSING){
		this->processTimers();
	}

	this->processStats();

	if(m_breakProcessing != EVENT_DISPATCHER_STATUS_BREAK_PROCESSING){
		return this->processNetwork(shouldIdle);
	}

	return 0;
}




}
}