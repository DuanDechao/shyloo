#include "slevent_poller.h"
#include "slpoller_select.h"
namespace sl
{
namespace network
{
#ifdef SL_OS_LINUX
#define HAS_EPOLL
#endif // SL_OS_LINUX

EventPoller::EventPoller()
	:m_fdReadHandlers(),
	 m_fdWriteHandlers(),
	 m_spareTime(0)
{}

EventPoller::~EventPoller()
{}

bool EventPoller::registerForRead(int fd, InputNotificationHandler* handler)
{
	if(!this->doRegisterForRead(fd))
	{
		return false;
	}

	m_fdReadHandlers[fd] = handler;
	return true;
}

bool EventPoller::registerForWrite(int fd, OutputNotificationHandler* handler)
{
	if(!this->doRegisterForWrite(fd))
	{
		return false;
	}
	m_fdWriteHandlers[fd] = handler;

	return true;
}

bool EventPoller::deregisterForRead(int fd)
{
	m_fdReadHandlers.erase(fd);
	return this->doDeregisterForRead(fd);
}

bool EventPoller::deregisterForWrite(int fd)
{
	m_fdWriteHandlers.erase(fd);
	return this->doDeregisterForWrite(fd);
}

bool EventPoller::triggerRead(int fd)
{
	FDReadHandlers::iterator iter = m_fdReadHandlers.find(fd);
	if(iter == m_fdReadHandlers.end())
	{
		return false;
	}

	iter->second->handleInputNotification(fd);
	return true;
}

bool EventPoller::triggerWrite(int fd)
{
	FDWriteHandlers::iterator iter = m_fdWriteHandlers.find(fd);

	if(iter == m_fdWriteHandlers.end())
	{
		return false;
	}

	iter->second->handleOutputNotification(fd);
	return true;
}

bool EventPoller::triggerError(int fd)
{
	if(!this->triggerRead(fd))
	{
		return this->triggerError(fd);
	}
	return true;
}

bool EventPoller::isRegistered(int fd, bool isForRead) const
{
	return isForRead ? (m_fdReadHandlers.find(fd) != m_fdReadHandlers.end()) :
		(m_fdWriteHandlers.find(fd) != m_fdWriteHandlers.end());
}

InputNotificationHandler* EventPoller::findForRead(int fd)
{
	FDReadHandlers::iterator iter = m_fdReadHandlers.find(fd);
	if(iter == m_fdReadHandlers.end())
		return NULL;

	return iter->second;
}

OutputNotificationHandler* EventPoller::findForWrite(int fd)
{
	FDWriteHandlers::iterator iter = m_fdWriteHandlers.find(fd);
	if(iter == m_fdWriteHandlers.end())
		return NULL;

	return iter->second;
}

int EventPoller::getFileDescriptor() const
{
	return -1;
}

int EventPoller::maxFD() const
{
	int readMaxFD = -1;

	FDReadHandlers::const_iterator iFDReadHandler = m_fdReadHandlers.begin();

	while(iFDReadHandler != m_fdReadHandlers.end())
	{
		if(iFDReadHandler->first > readMaxFD)
		{
			readMaxFD = iFDReadHandler->first;
		}

		++iFDReadHandler;
	}

	int writeMaxFD = -1;

	FDWriteHandlers::const_iterator iFDWriteHandler = m_fdWriteHandlers.begin();

	while(iFDWriteHandler != m_fdWriteHandlers.end())
	{
		if(iFDWriteHandler->first > writeMaxFD)
		{
			writeMaxFD = iFDWriteHandler->first;
		}
		++iFDWriteHandler;
	}

	return max(readMaxFD, writeMaxFD);
}

EventPoller* EventPoller::create()
{
#ifdef HAS_EPOLL
	return new EventPoller();
#else
	return new SelectPoller();
#endif // HAS_EPOLL
}

}
}