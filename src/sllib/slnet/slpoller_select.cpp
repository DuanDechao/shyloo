#include "slpoller_select.h"

namespace sl
{
namespace network
{
SelectPoller::SelectPoller()
	:EventPoller(),
	 m_fdReadSet(),
	 m_fdWriteSet(),
	 m_fdLargest(-1),
	 m_fdWriteCount(0)
{
	FD_ZERO(&m_fdReadSet);
	FD_ZERO(&m_fdWriteSet);
}

void SelectPoller::handleNotifications(int& countReady, fd_set& readFDs, fd_set& writeFDs)
{
#ifdef SL_OS_WINDOWS
	for (unsigned i = 0; i < readFDs.fd_count; i++)
	{
		int32 fd = (int32)readFDs.fd_array[i];
		--countReady;
		this->triggerRead(fd);
	}

	for (unsigned i = 0; i < writeFDs.fd_count; i++)
	{
		int32 fd = (int32)writeFDs.fd_array[i];
		--countReady;
		this->triggerWrite(fd);
	}
#else
	for (int fd = 0; fd < m_fdLargest && countReady > 0; ++fd)
	{
		if(FD_ISSET(fd, &readFDs))
		{
			--countReady;
			this->triggerRead(fd);
		}

		if(FD_ISSET(fd, &writeFDs))
		{
			--countReady;
			this->triggerWrite(fd);
		}
	}
#endif // SL_OS_WINDOWS
}

int SelectPoller::processPendingEvents(int64 maxWait)
{
	fd_set		readFDs;
	fd_set		writeFDs;
	struct timeval		nextTimeout;

	FD_ZERO(&readFDs);
	FD_ZERO(&writeFDs);

	readFDs = m_fdReadSet;
	writeFDs = m_fdWriteSet;

	nextTimeout.tv_sec = (int32)(maxWait/1000);
	nextTimeout.tv_usec = (int32)(((double)(maxWait / 1000) - (double)nextTimeout.tv_sec) * 1000000);

	int countReady = 0;

#ifdef SL_OS_WINDOWS
	if(m_fdLargest == -1)
	{
		Sleep(int32(maxWait));
	}
#endif // SL_OS_WINDOWS
	else
	{
		countReady = select(m_fdLargest + 1, &readFDs, m_fdWriteCount ? &writeFDs: NULL,
			NULL, &nextTimeout);
	}

	if(countReady > 0)
	{
		this->handleNotifications(countReady, readFDs, writeFDs);
	}
	else if(countReady == -1)
	{

	}
	return countReady;
}

bool SelectPoller::doRegisterForRead(int fd)
{
#ifdef SL_OS_WINDOWS
	if(m_fdReadSet.fd_count >= FD_SETSIZE)
	{
		return false;
	}
#else
	if((fd < 0) || (FD_SETSIZE <= fd))
	{
		return false;
	}
#endif // SL_OS_WINDOWS

	if(FD_ISSET(fd, &m_fdReadSet))
		return false;

	FD_SET(fd, &m_fdReadSet);

	if(fd > m_fdLargest)
	{
		m_fdLargest = fd;
	}

	return true;
}

bool SelectPoller::doRegisterForWrite(int fd)
{
#ifdef SL_OS_WINDOWS
	if(m_fdWriteSet.fd_count >= FD_SETSIZE)
	{
		return false;
	}
#else
	if((fd < 0) || (FD_SETSIZE <= fd))
	{
		return false;
	}
#endif // SL_OS_WINDOWS

	if(FD_ISSET(fd, &m_fdWriteSet))
	{
		return false;
	}

	FD_SET(fd, &m_fdWriteSet);

	if(fd > m_fdLargest)
	{
		m_fdLargest = fd;
	}

	++m_fdWriteCount;
	return true;
}

bool SelectPoller::doDeregisterForRead(int fd)
{
#ifdef SL_OS_LINUX
	if(fd < 0 || FD_SETSIZE <= fd)
	{
		return false;
	}
#endif
	if(!FD_ISSET(fd, &m_fdReadSet))
	{
		return false;
	}
	FD_CLR(fd, &m_fdReadSet);

	if(fd == m_fdLargest)
	{
		m_fdLargest = this->maxFD();
	}
	return true;
}

bool SelectPoller::doDeregisterForWrite(int fd)
{
#ifdef SL_OS_LINUX
	if(fd < 0 || FD_SETSIZE <= fd)
	{
		return false;
	}
#endif

	if(!FD_ISSET(fd, &m_fdWriteSet))
	{
		return false;
	}

	FD_CLR(fd, &m_fdWriteSet);

	if(fd == m_fdLargest)
	{
		m_fdLargest = this->maxFD();
	}

	m_fdWriteCount--;
	return true;
}
}
}