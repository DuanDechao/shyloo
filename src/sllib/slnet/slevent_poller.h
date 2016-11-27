#ifndef _SL_EVENT_POLLER_H_
#define _SL_EVENT_POLLER_H_
#include "slinterfaces.h"
#include "slnetbase.h"
#include <map>
namespace sl
{
namespace network
{
typedef std::map<int32, InputNotificationHandler *> FDReadHandlers;
typedef std::map<int32, OutputNotificationHandler *> FDWriteHandlers;

class EventPoller
{
public:
	EventPoller();
	virtual ~EventPoller();

	bool registerForRead(int32 fd, InputNotificationHandler* handler);
	bool registerForWrite(int32 fd, OutputNotificationHandler* handler);

	bool deregisterForRead(int32 fd);
	bool deregisterForWrite(int32 fd);

	virtual int32 processPendingEvents(double maxWait) = 0;
	virtual int32 getFileDescriptor() const;

	void clearSpareTime() {m_spareTime = 0;}
	uint64 spareTime() const {return m_spareTime;}

	static EventPoller* create();

	InputNotificationHandler* findForRead(int32 fd);
	OutputNotificationHandler* findForWrite(int32 fd);

protected:
	virtual bool doRegisterForRead(int32 fd) = 0;
	virtual bool doRegisterForWrite(int32 fd) = 0;

	virtual bool doDeregisterForRead(int32 fd) = 0;
	virtual bool doDeregisterForWrite(int32 fd) = 0;

	bool triggerRead(int32 fd);
	bool triggerWrite(int32 fd);
	bool triggerError(int32 fd);

	bool isRegistered(int32 fd, bool isForRead) const;

	int32 maxFD() const;

private:
	FDReadHandlers			m_fdReadHandlers;
	FDWriteHandlers			m_fdWriteHandlers;

protected:
	uint64					m_spareTime;


};

}
}
#endif