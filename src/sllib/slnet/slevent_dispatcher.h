#ifndef _SL_LIB_NET_EVENT_DISPATCHER_H_
#define _SL_LIB_NET_EVENT_DISPATCHER_H_
#include "slinterfaces.h"
#include "sltimer.h"

namespace sl{
namespace network{
class DispatcherCoupling;
class ErrorReporter;
class EventPoller;

class EventDispatcher{
public:
	enum EVENT_DISPATCHER_STATUS{
		EVENT_DISPATCHER_STATUS_RUNNING = 0,
		EVENT_DISPATCHER_STATUS_WAITING_BREAK_PROCESSING = 1,
		EVENT_DISPATCHER_STATUS_BREAK_PROCESSING = 2
	};

	EventDispatcher();
	virtual ~EventDispatcher();

	int processOnce(int64 overTime);
	void processUntilBreak(int64 overTime);

	inline bool hasBreakProcessing() const;
	inline bool waitingBreakProcessing() const;

	void breakProcessing(bool breakState = true);
	inline void setWaitBreakProcessing();

	bool registerReadFileDescriptor(int32 fd, InputNotificationHandler* handler);
	bool deregisterReadFileDescriptor(int32 fd);
	bool registerWriteFileDescriptor(int32 fd, OutputNotificationHandler* handler);
	bool deregisterWriteFileDescriptor(int32 fd);

	inline EventPoller* createPoller();
	inline EventPoller* pPoller() {return _pPoller;}

	int processNetwork(int64 overTime);

protected:
	int8			_breakProcessing;
	EventPoller*	_pPoller;

};
}
}
#include "slevent_dispatcher.inl"
#endif