#include "slevent_poller.h"
#include "slpoller_select.h"
namespace sl{
namespace network{
#ifdef SL_OS_LINUX
#define HAS_EPOLL
#endif // SL_OS_LINUX

EventPoller::EventPoller()
	:_fdReadHandlers(),
	 _fdWriteHandlers(),
	 _spareTime(0)
{}

EventPoller::~EventPoller()
{}

bool EventPoller::registerForRead(int fd, InputNotificationHandler* handler){
	if(!this->doRegisterForRead(fd)){
		return false;
	}

	_fdReadHandlers[fd] = handler;
	return true;
}

bool EventPoller::registerForWrite(int fd, OutputNotificationHandler* handler)
{
	if(!this->doRegisterForWrite(fd)){
		return false;
	}
	_fdWriteHandlers[fd] = handler;

	return true;
}

bool EventPoller::deregisterForRead(int fd){
	_fdReadHandlers.erase(fd);
	return this->doDeregisterForRead(fd);
}

bool EventPoller::deregisterForWrite(int fd){
	_fdWriteHandlers.erase(fd);
	//printf("deregister write fd[%d]-------------\n", fd);
	return this->doDeregisterForWrite(fd);
}

bool EventPoller::triggerRead(int fd){
	FDReadHandlers::iterator iter = _fdReadHandlers.find(fd);
	if(iter == _fdReadHandlers.end())
		return false;
	
	iter->second->handleInputNotification(fd);
	return true;
}

bool EventPoller::triggerWrite(int fd){
	FDWriteHandlers::iterator iter = _fdWriteHandlers.find(fd);

	if(iter == _fdWriteHandlers.end())
		return false;
	
	//printf("trigger write fd[%d %p]+++++++++\n",fd, iter->second);
	iter->second->handleOutputNotification(fd);
	return true;
}

bool EventPoller::triggerError(int fd){
	if(!this->triggerRead(fd)){
		return this->triggerError(fd);
	}
	return true;
}

bool EventPoller::isRegistered(int fd, bool isForRead) const{
	return isForRead ? (_fdReadHandlers.find(fd) !=_fdReadHandlers.end()) :
		(_fdWriteHandlers.find(fd) != _fdWriteHandlers.end());
}

InputNotificationHandler* EventPoller::findForRead(int fd){
	FDReadHandlers::iterator iter = _fdReadHandlers.find(fd);
	if(iter == _fdReadHandlers.end())
		return NULL;

	return iter->second;
}

OutputNotificationHandler* EventPoller::findForWrite(int fd){
	FDWriteHandlers::iterator iter = _fdWriteHandlers.find(fd);
	if(iter == _fdWriteHandlers.end())
		return NULL;

	return iter->second;
}

int EventPoller::getFileDescriptor() const{
	return -1;
}

int EventPoller::maxFD() const{
	int readMaxFD = -1;
	FDReadHandlers::const_iterator iFDReadHandler = _fdReadHandlers.begin();
	while(iFDReadHandler != _fdReadHandlers.end()){
		if(iFDReadHandler->first > readMaxFD){
			readMaxFD = iFDReadHandler->first;
		}

		++iFDReadHandler;
	}

	int writeMaxFD = -1;
	FDWriteHandlers::const_iterator iFDWriteHandler = _fdWriteHandlers.begin();
	while(iFDWriteHandler != _fdWriteHandlers.end()){
		if(iFDWriteHandler->first > writeMaxFD){
			writeMaxFD = iFDWriteHandler->first;
		}
		++iFDWriteHandler;
	}

	return readMaxFD > writeMaxFD ? readMaxFD : writeMaxFD;
}

EventPoller* EventPoller::create()
{
/*#ifdef HAS_EPOLL
	return NEW EventPoller();
#else
	return NEW SelectPoller();
#endif // HAS_EPOLL*/
	return NEW SelectPoller();
}

}
}
