#ifndef _SL_LIB_NET_EVENT_POLLER_H_
#define _SL_LIB_NET_EVENT_POLLER_H_
#include "slinterfaces.h"
#include "slnetbase.h"
#include <map>
namespace sl{
namespace network{

#ifdef SL_OS_LINUX
#define HAS_EPOLL
#endif // SL_OS_LINUX

class EventPoller{
public:
	typedef struct tagFDHandler{
		tagFDHandler() :fd(0),readHandler(nullptr), writeHandler(nullptr){}
		int32 fd;
		InputNotificationHandler* readHandler;
		OutputNotificationHandler* writeHandler;
	}FDHandler;

	typedef std::map<int32, FDHandler> FDHandlerMap;

	EventPoller();
	virtual ~EventPoller();

	bool registerForRead(int32 fd, InputNotificationHandler* handler);
	bool registerForWrite(int32 fd, OutputNotificationHandler* handler);

	bool deregisterForRead(int32 fd);
	bool deregisterForWrite(int32 fd);

	virtual int32 processPendingEvents(int64 maxWait) = 0;
	virtual int32 getFileDescriptor() const;

	inline void clearSpareTime() {_spareTime = 0;}
	inline uint64 spareTime() const {return _spareTime;}

	static EventPoller* create();

	InputNotificationHandler* findForRead(int32 fd);
	OutputNotificationHandler* findForWrite(int32 fd);

protected:
	virtual bool doRegisterForRead(int32 fd, void* handler) = 0;
	virtual bool doRegisterForWrite(int32 fd, void* handler) = 0;

	virtual bool doDeregisterForRead(int32 fd, void* handler) = 0;
	virtual bool doDeregisterForWrite(int32 fd, void* handler) = 0;

	bool triggerRead(int32 fd, void* handler);
	bool triggerWrite(int32 fd, void* handler);
	bool triggerError(int32 fd, void* handler);

	bool isRegistered(int32 fd, bool isForRead) const;

	int32 maxFD() const;

private:
	FDHandlerMap			_fdHandlers;

protected:
	uint64					_spareTime;
};

}
}
#endif
