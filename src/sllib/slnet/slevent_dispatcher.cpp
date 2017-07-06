#include "slevent_dispatcher.h"
#include "slevent_poller.h"
#include "sltime.h"
namespace sl{
namespace network{

EventDispatcher::EventDispatcher()
	:_breakProcessing(EVENT_DISPATCHER_STATUS_RUNNING)
{
	_pPoller = EventPoller::create();
}

EventDispatcher::~EventDispatcher(){
	SAFE_RELEASE(_pPoller);
}

EventPoller* EventDispatcher::createPoller(){
	_pPoller = EventPoller::create();
	return _pPoller;
}

bool EventDispatcher::registerReadFileDescriptor(int fd, InputNotificationHandler* handler){
	return _pPoller->registerForRead(fd, handler);
}

bool EventDispatcher::registerWriteFileDescriptor(int fd, OutputNotificationHandler* handler){
	return _pPoller->registerForWrite(fd, handler);
}

bool EventDispatcher::deregisterReadFileDescriptor(int fd){
	return _pPoller->deregisterForRead(fd);
}

bool EventDispatcher::deregisterWriteFileDescriptor(int fd){
	return _pPoller->deregisterForWrite(fd);
}

int EventDispatcher::processNetwork(int64 overTime){
	return _pPoller->processPendingEvents(overTime);
}

void EventDispatcher::processUntilBreak(int64 overTime){
	if(_breakProcessing != EVENT_DISPATCHER_STATUS_BREAK_PROCESSING)
		_breakProcessing = EVENT_DISPATCHER_STATUS_RUNNING;
	
	int64 tick = sl::getTimeMilliSecond();
	while(_breakProcessing != EVENT_DISPATCHER_STATUS_BREAK_PROCESSING){
		this->processOnce(overTime);
		if ((sl::getTimeMilliSecond() - tick) >= overTime){
			breakProcessing(true);
		}
	}
}

int EventDispatcher::processOnce(int64 overTime){
	if(_breakProcessing != EVENT_DISPATCHER_STATUS_BREAK_PROCESSING)
		_breakProcessing = EVENT_DISPATCHER_STATUS_RUNNING;

	if(_breakProcessing != EVENT_DISPATCHER_STATUS_BREAK_PROCESSING){
		return this->processNetwork(overTime);
	}

	return 0;
}




}
}
