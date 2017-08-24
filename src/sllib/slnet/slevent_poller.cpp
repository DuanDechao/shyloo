#include "slevent_poller.h"
#include "slpoller_select.h"
#include "slpoller_epoll.h"
namespace sl{
namespace network{

EventPoller::EventPoller()
	:_fdHandlers(),
	 _spareTime(0)
{}

EventPoller::~EventPoller()
{}

bool EventPoller::registerForRead(int fd, InputNotificationHandler* handler){
	_fdHandlers[fd].fd = fd;
	_fdHandlers[fd].readHandler = handler;
	if (!this->doRegisterForRead(fd, &_fdHandlers[fd])){
		return false;
	}
	return true;
}

bool EventPoller::registerForWrite(int fd, OutputNotificationHandler* handler){
	_fdHandlers[fd].fd = fd;
	_fdHandlers[fd].writeHandler = handler;
	if (!this->doRegisterForWrite(fd, &_fdHandlers[fd])){
		return false;
	}

	return true;
}

bool EventPoller::deregisterForRead(int fd){
	_fdHandlers[fd].readHandler = nullptr;
	return this->doDeregisterForRead(fd, &_fdHandlers[fd]);
}

bool EventPoller::deregisterForWrite(int fd){
	_fdHandlers[fd].writeHandler = nullptr;
	return this->doDeregisterForWrite(fd, &_fdHandlers[fd]);
}

bool EventPoller::triggerRead(int fd, void* handler){
	FDHandler* fdHandler = nullptr;
	if (handler != nullptr){
		fdHandler = (FDHandler*)handler;
	}
	else{
		FDHandlerMap::iterator iter = _fdHandlers.find(fd);
		if (iter == _fdHandlers.end())
			return false;

		fdHandler = &(iter->second);
	}
	
	if (fdHandler->readHandler == nullptr){
		return false;
	}
	
	fdHandler->readHandler->handleInputNotification(fdHandler->fd);
	return true;
}

bool EventPoller::triggerWrite(int fd, void* handler){
	FDHandler* fdHandler = nullptr;
	if (handler != nullptr){
		fdHandler = (FDHandler*)handler;
	}
	else{
		FDHandlerMap::iterator iter = _fdHandlers.find(fd);
		if (iter == _fdHandlers.end())
			return false;

		fdHandler = &(iter->second);
	}

	if (fdHandler->writeHandler == nullptr){
		return false;
	}

	fdHandler->writeHandler->handleOutputNotification(fdHandler->fd);
	return true;
}

bool EventPoller::triggerError(int fd, void* handler){
	if (!this->triggerRead(fd, handler)){
		return this->triggerWrite(fd, handler);
	}
	return true;
}

bool EventPoller::isRegistered(int fd, bool isForRead) const{
	auto itor = _fdHandlers.find(fd);
	if (itor == _fdHandlers.end())
		return false;

	return isForRead ? (itor->second.readHandler != nullptr) :
		(itor->second.writeHandler != nullptr);
}

InputNotificationHandler* EventPoller::findForRead(int fd){
	FDHandlerMap::iterator iter = _fdHandlers.find(fd);
	if (iter == _fdHandlers.end())
		return NULL;

	return iter->second.readHandler;
}

OutputNotificationHandler* EventPoller::findForWrite(int fd){
	FDHandlerMap::iterator iter = _fdHandlers.find(fd);
	if (iter == _fdHandlers.end())
		return NULL;

	return iter->second.writeHandler;
}

int EventPoller::getFileDescriptor() const{
	return -1;
}

int EventPoller::maxFD() const{
	int MaxFD = -1;
	FDHandlerMap::const_iterator iFDReadHandler = _fdHandlers.begin();
	while (iFDReadHandler != _fdHandlers.end()){
		if (iFDReadHandler->first > MaxFD){
			MaxFD = iFDReadHandler->first;
		}

		++iFDReadHandler;
	}
	return MaxFD;
}

EventPoller* EventPoller::create()
{
#ifdef HAS_EPOLL
	return NEW EpollPoller();
#else
	return NEW SelectPoller();
#endif // HAS_EPOLL
}

}
}
