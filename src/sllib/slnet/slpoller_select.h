#ifndef _SL_SELECT_POLLER_H_
#define _SL_SELECT_POLLER_H_

#include "slevent_poller.h"

namespace sl
{
namespace network
{
#ifndef HAS_EPOLL
class SelectPoller: public EventPoller
{
public:
	SelectPoller();

protected:
	virtual bool doRegisterForRead(int fd);
	virtual bool doRegisterForWrite(int fd);

	virtual bool doDeregisterForRead(int fd);
	virtual bool doDeregisterForWrite(int fd);

	virtual int processPendingEvents(int64 maxWait);

private:
	void handleNotifications(int& countReady, fd_set& readFDs, fd_set& writeFDs);

	fd_set				m_fdReadSet;
	fd_set				m_fdWriteSet;


	int					m_fdLargest;
	int					m_fdWriteCount;
};
#endif // !HAS_EPOLL

}
}
#endif