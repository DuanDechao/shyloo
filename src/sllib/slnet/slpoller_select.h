#ifndef _SL_LIB_NET_SELECT_POLLER_H_
#define _SL_LIB_NET_SELECT_POLLER_H_

#include "slevent_poller.h"

namespace sl{
namespace network{

#ifndef HAS_EPOLL
class SelectPoller: public EventPoller{
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

private:
	fd_set				_fdReadSet;
	fd_set				_fdWriteSet;


	int					_fdLargest;
	int					_fdWriteCount;
};
#endif // !HAS_EPOLL

}
}
#endif