#include "slevent_dispatcher.h"
namespace sl { 
namespace network
{

inline void EventDispatcher::breakProcessing(bool breakState){
	if(breakState)
		_breakProcessing = EVENT_DISPATCHER_STATUS_BREAK_PROCESSING;
	else
		_breakProcessing = EVENT_DISPATCHER_STATUS_RUNNING;
}

inline void EventDispatcher::setWaitBreakProcessing(){
	_breakProcessing = EVENT_DISPATCHER_STATUS_WAITING_BREAK_PROCESSING;
}

inline bool EventDispatcher::hasBreakProcessing() const { 
	return _breakProcessing == EVENT_DISPATCHER_STATUS_BREAK_PROCESSING; 
}

inline bool EventDispatcher::waitingBreakProcessing() const { 
	return _breakProcessing == EVENT_DISPATCHER_STATUS_WAITING_BREAK_PROCESSING; 
}

} // namespace Network
}
// event_dispatcher.inl
