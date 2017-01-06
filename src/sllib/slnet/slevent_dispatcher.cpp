#include "slevent_dispatcher.h"
#include "slevent_poller.h"
#include "sltime.h"
namespace sl
{
namespace network
{
EventDispatcher::EventDispatcher()
	:m_breakProcessing(EVENT_DISPATCHER_STATUS_RUNNING)
{
	m_pPoller = EventPoller::create();
}

EventDispatcher::~EventDispatcher()
{
	SAFE_RELEASE(m_pPoller);
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

int EventDispatcher::processNetwork(int64 overTime)
{
	return m_pPoller->processPendingEvents(overTime);
}

void EventDispatcher::processUntilBreak(int64 overTime)
{
	if(m_breakProcessing != EVENT_DISPATCHER_STATUS_BREAK_PROCESSING)
		m_breakProcessing = EVENT_DISPATCHER_STATUS_RUNNING;
	
	int64 tick = sl::getTimeMilliSecond();
	while(m_breakProcessing != EVENT_DISPATCHER_STATUS_BREAK_PROCESSING){
		this->processOnce(overTime);
		if ((sl::getTimeMilliSecond() - tick) >= (uint64)overTime){
			breakProcessing(true);
		}
	}
}

int EventDispatcher::processOnce(int64 overTime)
{
	if(m_breakProcessing != EVENT_DISPATCHER_STATUS_BREAK_PROCESSING)
		m_breakProcessing = EVENT_DISPATCHER_STATUS_RUNNING;

	if(m_breakProcessing != EVENT_DISPATCHER_STATUS_BREAK_PROCESSING){
		return this->processNetwork(overTime);
	}

	return 0;
}




}
}