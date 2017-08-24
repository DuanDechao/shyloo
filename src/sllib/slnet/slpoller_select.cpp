#include "slpoller_select.h"

namespace sl{
namespace network{
SelectPoller::SelectPoller()
	:EventPoller(),
	 _fdReadSet(),
	 _fdWriteSet(),
	 _fdLargest(-1),
	 _fdWriteCount(0)
{
	FD_ZERO(&_fdReadSet);
	FD_ZERO(&_fdWriteSet);
}

void SelectPoller::handleNotifications(int& countReady, fd_set& readFDs, fd_set& writeFDs){
#ifdef SL_OS_WINDOWS
	for (unsigned i = 0; i < readFDs.fd_count; i++){
		int32 fd = (int32)readFDs.fd_array[i];
		--countReady;
		this->triggerRead(fd, nullptr);
	}

	for (unsigned i = 0; i < writeFDs.fd_count; i++){
		int32 fd = (int32)writeFDs.fd_array[i];
		--countReady;
		this->triggerWrite(fd, nullptr);
	}
#else
	for (int fd = 0; fd <= _fdLargest && countReady > 0; ++fd){
		if(FD_ISSET(fd, &readFDs)){
			--countReady;
			this->triggerRead(fd, nullptr);
		}

		if(FD_ISSET(fd, &writeFDs)){
			--countReady;
			this->triggerWrite(fd, nullptr);
		}
	}
#endif // SL_OS_WINDOWS
}

int SelectPoller::processPendingEvents(int64 maxWait){
	fd_set		readFDs;
	fd_set		writeFDs;
	struct timeval		nextTimeout;

	FD_ZERO(&readFDs);
	FD_ZERO(&writeFDs);

	readFDs = _fdReadSet;
	writeFDs = _fdWriteSet;

	nextTimeout.tv_sec = (int32)(maxWait/1000);
	nextTimeout.tv_usec = (int32)(((double)(maxWait / 1000) - (double)nextTimeout.tv_sec) * 1000000);

	int countReady = 0;

#ifdef SL_OS_WINDOWS
	if(_fdLargest == -1){
		Sleep(int32(maxWait));
	}else
#endif // SL_OS_WINDOWS
	{
		countReady = select(_fdLargest + 1, &readFDs, _fdWriteCount ? &writeFDs: NULL,
			NULL, &nextTimeout);
	}

	if(countReady > 0){
		this->handleNotifications(countReady, readFDs, writeFDs);
	}
	else if(countReady == -1){

	}

	return countReady;
}

bool SelectPoller::doRegisterForRead(int fd, void* handler){
#ifdef SL_OS_WINDOWS
	if(_fdReadSet.fd_count >= FD_SETSIZE){
		return false;
	}
#else
	if((fd < 0) || (FD_SETSIZE <= fd))
	{
		return false;
	}
#endif // SL_OS_WINDOWS

	if(FD_ISSET(fd, &_fdReadSet))
		return false;

	FD_SET(fd, &_fdReadSet);

	if(fd > _fdLargest){
		_fdLargest = fd;
	}

	return true;
}

bool SelectPoller::doRegisterForWrite(int fd, void* handler){
#ifdef SL_OS_WINDOWS
	if(_fdWriteSet.fd_count >= FD_SETSIZE){
		return false;
	}
#else
	if((fd < 0) || (FD_SETSIZE <= fd)){
		return false;
	}
#endif // SL_OS_WINDOWS

	if(FD_ISSET(fd, &_fdWriteSet)){
		return false;
	}

	FD_SET(fd, &_fdWriteSet);

	if(fd > _fdLargest){
		_fdLargest = fd;
	}

	++_fdWriteCount;
	return true;
}

bool SelectPoller::doDeregisterForRead(int fd, void* handler){
#ifdef SL_OS_LINUX
	if(fd < 0 || FD_SETSIZE <= fd){
		return false;
	}
#endif
	if(!FD_ISSET(fd, &_fdReadSet)){
		return false;
	}
	FD_CLR(fd, &_fdReadSet);

	if(fd == _fdLargest){
		_fdLargest = this->maxFD();
	}
	return true;
}

bool SelectPoller::doDeregisterForWrite(int fd, void* handler){
#ifdef SL_OS_LINUX
	if(fd < 0 || FD_SETSIZE <= fd){
		return false;
	}
#endif

	if(!FD_ISSET(fd, &_fdWriteSet)){
		return false;
	}

	FD_CLR(fd, &_fdWriteSet);

	if(fd == _fdLargest){
		_fdLargest = this->maxFD();
	}

	_fdWriteCount--;
	return true;
}
}
}
