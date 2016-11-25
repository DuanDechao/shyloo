#include "slevent_dispatcher.h"
namespace sl { 
namespace network
{

inline void EventDispatcher::breakProcessing(bool breakState)
{
	if(breakState)
		m_breakProcessing = EVENT_DISPATCHER_STATUS_BREAK_PROCESSING;
	else
		m_breakProcessing = EVENT_DISPATCHER_STATUS_RUNNING;
}

inline void EventDispatcher::setWaitBreakProcessing()
{
	m_breakProcessing = EVENT_DISPATCHER_STATUS_WAITING_BREAK_PROCESSING;
}

inline bool EventDispatcher::hasBreakProcessing() const 
{ 
	return m_breakProcessing == EVENT_DISPATCHER_STATUS_BREAK_PROCESSING; 
}

inline bool EventDispatcher::waitingBreakProcessing() const 
{ 
	return m_breakProcessing == EVENT_DISPATCHER_STATUS_WAITING_BREAK_PROCESSING; 
}

inline double EventDispatcher::maxWait() const
{
	return m_maxWait;
}

inline void EventDispatcher::maxWait(double seconds)
{
	m_maxWait = seconds;
}

} // namespace Network
}
// event_dispatcher.inl
