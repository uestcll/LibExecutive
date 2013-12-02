#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "CLEpoll.h"
#include "CLLogger.h"

CLEpoll::CLEpoll()
{
	m_pEpollEvents = NULL;
	m_iEpollFd = -1;
	m_iMaxEventSize = 0;
}

CLEpoll::~CLEpoll()
{

}

CLStatus CLEpoll::Initialize(int maxFdSize)
{
	m_iMaxEventSize = maxFdSize;

	m_iEpollFd = epoll_create(maxFdSize);
	if(0 > m_iEpollFd)
	{	
		CLLogger::WriteLogMsg("In CLEpoll::Initialize(), epoll_create error", errno);
		return CLStatus(-1, errno);
	}

	m_pEpollEvents = new epoll_event[m_iMaxEventSize];
	memset(m_pEpollEvents, 0, m_iMaxEventSize * sizeof(struct epoll_event));

	// m_pMsgLoopManager = pMsgLoopManager;

	return CLStatus(0, 0);
}

CLStatus CLEpoll::DoEvent(CLEpollEvent *pEvent, int fd, int epollOpt, int epollEvents)
{
	struct epoll_event ev;
	memset(&ev, 0, sizeof(struct epoll_event));
	ev.data.ptr = pEvent->GetHandlePtr();
	ev.events = epollEvents;

	if(0 > epoll_ctl(m_iEpollFd, epollOpt, fd, &ev))
	{
		CLLogger::WriteLogMsg("In CLEpoll::DoEvent(), epoll_ctl() error", errno);
		return CLStatus(-1, errno);
	}

	return CLStatus(0, 0);
}


CLStatus CLEpoll::Run()
{
	while(1)
	{
		
	}


	return CLStatus(0, 0);
}

