#ifndef _SL_EVENT_POLLER_H_
#define _SL_EVENT_POLLER_H_
#include "slbase.h"
#include "slinterfaces.h"
#include "slnetbase.h"

namespace sl
{
namespace network
{
typedef std::map<int, InputNotificationHandler *> FDReadHandlers;
typedef std::map<int, OutputNotificationHandler *> FDWriteHandlers;

class EventPoller
{
public:
	EventPoller();
	virtual ~EventPoller();

	bool registerForRead(int fd, InputNotificationHandler* handler);
	bool registerForWrite(int fd, OutputNotificationHandler* handler);

	bool deregisterForRead(int fd);
	bool deregisterForWrite(int fd);

	virtual int processPendingEvents(double maxWait) = 0;
	virtual int getFileDescriptor() const;

	void clearSpareTime() {m_spareTime = 0;}
	uint64 spareTime() const {return m_spareTime;}

	static EventPoller* create();

	InputNotificationHandler* findForRead(int fd);
	OutputNotificationHandler* findForWrite(int fd);

protected:
	virtual bool doRegisterForRead(int fd) = 0;
	virtual bool doRegisterForWrite(int fd) = 0;

	virtual bool doDeregisterForRead(int fd) = 0;
	virtual bool doDeregisterForWrite(int fd) = 0;

	bool triggerRead(int fd);
	bool triggerWrite(int fd);
	bool triggerError(int fd);

	bool isRegistered(int fd, bool isForRead) const;

	int maxFD() const;

private:
	FDReadHandlers			m_fdReadHandlers;
	FDWriteHandlers			m_fdWriteHandlers;

protected:
	uint64					m_spareTime;


};

}
}
#endif