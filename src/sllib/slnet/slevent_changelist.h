#ifndef _SL_LIB_NET_EVENT_CHANGELIST_H_
#define _SL_LIB_NET_EVENT_CHANGELIST_H_
#include "slmulti_sys.h"

namespace sl{
namespace network{
typedef struct tagEventChange{
	int32 fd;
	int16 oldEvents;
	uint8 readChange;
	uint8 writeChange;
	uint8 closeChange;
}EventChange;

typedef struct tagEventChangeListFdInfo{
	int32 idxplus1;
}EventChangeListFdInfo;

#define EV_TIMEOUT	0x01
/** Wait for a socket or FD to become readable */
#define EV_READ		0x02
/** Wait for a socket or FD to become writeable */
#define EV_WRITE	0x04
/** Wait for a POSIX signal to be raised*/
#define EV_SIGNAL	0x08
/**
* Persistent event: won't get removed automatically when activated.
*
* When a persistent event with a timeout becomes activated, its timeout
* is reset to 0.
*/
#define EV_PERSIST	0x10
/** Select edge-triggered behavior, if supported by the backend. */
#define EV_ET		0x20


/* If set, add the event. */
#define EV_CHANGE_ADD     0x01
/* If set, delete the event.  Exclusive with EV_CHANGE_ADD */
#define EV_CHANGE_DEL     0x02
/* If set, this event refers a signal, not an fd. */
#define EV_CHANGE_SIGNAL  EV_SIGNAL
/* Set for persistent events.  Currently not used. */
#define EV_CHANGE_PERSIST EV_PERSIST
/* Set for adding edge-triggered events. */
#define EV_CHANGE_ET      EV_ET

#define EV_FINALIZE     0x40
/**
* Detects connection close events.  You can use this to detect when a
* connection has been closed, without having to read all the pending data
* from a connection.
*
* Not all backends support EV_CLOSED.  To detect or require it, use the
* feature flag EV_FEATURE_EARLY_CLOSE.
**/
#define EV_CLOSED	0x80
/**@}*/

class EventDispatcher;
class EventChangeList{
public:
	EventChangeList() :_changes(nullptr), _changesNum(0), _changesSize(0){}
	~EventChangeList(){}

	inline void init(){
		_changes = nullptr;
		_changesNum = 0;
		_changesSize = 0;
	}

	EventChange* EventChangeListGetOrConstruct(int32 fd, int16 oldEvents, EventChangeListFdInfo *fdinfo);
	int32 EventChangeListAdd(int32 fd, int16 old, int16 events, void* fdinfo);
	int32 EventChangeListDel(int32 fd, int16 old, int16 events, void* fdinfo);
	inline int32 changesNum() const { return _changesNum; }
	EventChange* getChange(int32 idx) { return (idx < _changesNum ? &_changes[idx] : nullptr) ; }
	void EventChangeListRemoveAll();

private:
	int32 ChangeListGrow();

private:
	EventChange*		_changes;
	int32				_changesNum;
	int32				_changesSize;
	EventDispatcher*    _eventDispatcher;
};
}
}
#endif