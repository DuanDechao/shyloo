#include "slpoller_epoll.h"

namespace sl{
namespace network{
#ifdef HAS_EPOLL
#include <sys/epoll.h>
EpollPoller::EpollPoller(int expectedSize)
	:EventPoller(),
	_epFd(epoll_create(expectedSize))
{
	if(_epFd == -1){
		printf("create epoll fd failed\n");
	}
}

EpollPoller::~EpollPoller(){
	if(_epFd != -1){
		close(_epFd);
	}
}

int EpollPoller::processPendingEvents(int64 maxWait){
	const int MAX_EVENTS = 50;
	struct epoll_event events[MAX_EVENTS];
	int maxWaitInMilliseconds = int(maxWait * 1000);

	int nfds = epoll_wait(_epFd, events, MAX_EVENTS, maxWaitInMilliseconds);

	for (int i = 0; i < nfds; i++){
		if(events[i].events & (EPOLLERR|EPOLLHUP)){
			this->triggerError(events[i].data.fd, events[i].data.ptr);
		}
		else{
			if (events[i].events & EPOLLIN){
				this->triggerRead(events[i].data.fd, events[i].data.ptr);
            }

			if (events[i].events & EPOLLOUT){
				this->triggerWrite(events[i].data.fd, events[i].data.ptr);
			}
		}
	}
	return nfds;
}

bool EpollPoller::doRegister(int fd, bool isRead, bool isRegister, void* handler){
	struct epoll_event evt;
	memset(&evt, 0, sizeof(evt));
    evt.data.ptr = handler;
	
	int32 op;
	if(this->isRegistered(fd, !isRead)){
		op = EPOLL_CTL_MOD;
		evt.events = isRegister ? EPOLLIN|EPOLLOUT : 
			isRead ? EPOLLOUT : EPOLLIN;
	}else{
		evt.events = isRead ? EPOLLIN : EPOLLOUT;
		op = isRegister ? EPOLL_CTL_ADD : EPOLL_CTL_DEL;
    }

	if (epoll_ctl(_epFd, op, fd, &evt) < 0){
		printf("EpollPoller::doRegister: Failed to %s %s fd %d\n", isRegister ? "add" : "remove", isRead ? "read" : "write", fd);
		return false;
	}
	return true;
}


#endif // !HAS_EPOLL

}
}
