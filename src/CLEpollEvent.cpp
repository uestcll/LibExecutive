#include <sys/epoll.h>
#include <fnctl.h>
#include <stdio.h>

#include "CLEpollEvent.h"
#include "CLLogger.h"
#include "CLEpoll.h"

#define EVENTADD                     EPOLL_CTL_ADD
#define EVENTDEL                     EPOLL_CTL_DEL
#define EVENTCHANGE                  EPOLL_CTL_MOD

#define EDGEREVENT                  (EPOLLIN | EPOLLET)
#define EDGEWEVENT                  (EPOLLOUT | EPOLLET)
#define EDGEEVENTS                  (EPOLLIN |EPOLLOUT| EPOLLERR | EPOLLHUP | EPOLLET)

#define LEVELREVENT                  EPOLLIN
#define LEVELWEVENT                  EPOLLOUT
#define LEVELEVENTS                  (EPOLLIN |EPOLLOUT| EPOLLERR | EPOLLHUP)


CLEpollEvent::CLEpollEvent(CLEpoll *pEpoll)
{
	m_pEpoll = pEpoll;
	m_Fd = -1;
	m_pHandler = NULL;
}

CLEpollEvent::~CLEpollEvent()
{

}

CLStatus CLEpollEvent::SetFd(int fd)
{
	m_Fd = fd;
	return CLStatus(0, 0);
}

CLStatus CLEpollEvent::SetHandler(void* pHandler)
{
	m_pHandler = pHandler;

	return CLStatus(0, 0);
}

CLStatus CLEpollEvent::RegisterREvent()
{
	if(-1 == m_Fd || (NULL == m_pHandler))
	{
		CLLogger::WriteLogMsg("In CLEpollEvent::RegisterREvent(), m_Fd = -1 | m_pHandler = NULL", 0);
		return CLStatus(-1, 0);
	}

	CLStatus s = m_pEpoll->DoEvent(this, m_Fd, EVENTADD, EDGEEVENTS);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLEpollEvent::RegisterREvent(), m_pEpoll->DoEvent() error", 0);
		return CLStatus(-1, 0);
	}

	return CLStatus(0, 0);
}


