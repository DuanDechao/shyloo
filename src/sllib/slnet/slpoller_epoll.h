#ifndef _SL_LIB_NET_EPOLL_POLLER_H_
#define _SL_LIB_NET_EPOLL_POLLER_H_

#include "slevent_poller.h"

namespace sl{
namespace network{

#ifdef HAS_EPOLL
class EpollPoller: public EventPoller{
public:
	EpollPoller(int expectedSize = 10);
	virtual ~EpollPoller();

protected:
	virtual bool doRegisterForRead(int fd, void* handler){return doRegister(fd, true, true, handler);}
	virtual bool doRegisterForWrite(int fd, void* handler){return doRegister(fd, false, true, handler);}

	virtual bool doDeregisterForRead(int fd, void* handler){return doRegister(fd, true, false, handler);}
	virtual bool doDeregisterForWrite(int fd, void* handler){return doRegister(fd, false, false, handler);}

	virtual int processPendingEvents(int64 maxWait);

private:
	bool doRegister(int fd, bool isRead, bool isRegister, void* handler);

private:
	int				_epFd;
};
#endif // !HAS_EPOLL

}
}
#endif
