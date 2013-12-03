#include <sys/epoll.h>
#include <fnctl.h>
#include <stdio.h>

#include "CLEpollEvent.h"
#include "CLLogger.h"
#include "CLEpoll.h"

#define EVENTADD                    EPOLL_CTL_ADD
#define EVENTDEL                    EPOLL_CTL_DEL
#define EVENTCHANGE                 EPOLL_CTL_MOD

#define EDGEMODE					EPOLLET

#define LEVELREVENT                 EPOLLIN
#define LEVELWEVENT                 EPOLLOUT
#define LEVELEVENTS                 (EPOLLIN |EPOLLOUT| EPOLLERR | EPOLLHUP)


CLEpollEvent::CLEpollEvent(CLEpoll *pEpoll)
{
	m_pEpoll = pEpoll;
	m_Fd = -1;
	m_pHandler = NULL;
	m_bReadFlag = m_bWriteFlag = false;
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

void* CLEpollEvent::GetHandler()
{
	return m_pHandler;
}

CLStatus CLEpollEvent::RegisterREvents()
{
	if(-1 == m_Fd || (NULL == m_pHandler))
	{
		CLLogger::WriteLogMsg("In CLEpollEvent::RegisterREvent(), m_Fd = -1 | m_pHandler = NULL", 0);
		return CLStatus(-1, 0);
	}

	if(m_bReadFlag)
		return CLStatus(0, 0);

	m_Event = LEVELREVENT | EDGEMODE;
	CLStatus s = m_pEpoll->DoEvent(this, m_Fd, EVENTADD, m_Event);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLEpollEvent::RegisterREvent(), m_pEpoll->DoEvent() error", 0);
		return CLStatus(-1, 0);
	}
	
	m_bReadFlag = true;
	return CLStatus(0, 0);
}

CLStatus CLEpollEvent::RegisterWEvent()
{
	if(-1 == m_Fd || (NULL == m_pHandler))
	{
		CLLogger::WriteLogMsg("In CLEpollEvent::RegisterWEvent(), m_Fd = -1 | m_pHandler = NULL", 0);
		return CLStatus(-1, 0);
	}
	if(m_bWriteFlag)
		return CLStatus(0, 0);

	m_Event = LEVELWEVENT | EDGEMODE;
	CLStatus s = m_pEpoll->DoEvent(this, m_Fd, EVENTADD, m_Event);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLEpollEvent::RegisterWEvent(), m_pEpoll->DoEvent() error", 0);
		return CLStatus(-1, 0);
	}
	
	m_bWriteFlag = true;

	return CLStatus(0, 0);
}

CLStatus CLEpollEvent::RegisterRWEvent()
{
	if(-1 == m_Fd || (NULL == m_pHandler))
	{
		CLLogger::WriteLogMsg("In CLEpollEvent::RegisterRWEvent(), m_Fd = -1 | m_pHandler = NULL", 0);
		return CLStatus(-1, 0);
	}
	if(m_bWriteFlag && m_bReadFlag)
	{
		return CLStatus(0, 0);
	}	

	m_Event = LEVELEVENTS | EDGEMODE;
	CLStatus s = m_pEpoll->DoEvent(this, m_Fd, EVENTADD, m_Event);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLEpollEvent::RegisterRWEvent(), m_pEpoll->DoEvent() error", 0);
		return CLStatus(-1, 0);
	}

	m_bWriteFlag = m_bReadFlag = true;

	return CLStatus(0, 0);
}

CLStatus CLEpollEvent::UnRegisterRWEvents()
{
	if(-1 == m_Fd || (NULL == m_pHandler))
	{
		CLLogger::WriteLogMsg("In CLEpollEvent::UnRegisterRWEvents(), m_Fd = -1 | m_pHandler = NULL", 0);
		return CLStatus(-1, 0);
	}

	m_Event = LEVELREVENT | EDGEMODE;
	m_bReadFlag = false;
	m_bWriteFlag = false;

	CLStatus s = m_pEpoll->DoEvent(this, m_Fd, EVENTDEL, m_Event);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLEpollEvent::UnRegisterRWEvents(), m_pEpoll->DoEvent() error", 0);
		return CLStatus(-1, 0);
	}

	return CLStatus(0, 0);
}

CLStatus CLEpollEvent::OpenREvent()
{
	if(m_bReadFlag)
		return CLStatus(0, 0);

	m_Event |= (LEVELREVENT | EDGEMODE);

	CLStatus s = m_pEpoll->DoEvent(this, m_Fd, EVENTCHANGE, m_Event);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLEpollEvent::OpenREvent(), m_pEpoll->DoEvent() error", 0);
		return CLStatus(-1, 0);
	}

	m_bReadFlag = true;
	return CLStatus(0, 0);
}

CLStatus CLEpollEvent::OpenWEvent()
{
	if(m_bWriteFlag)
		return CLStatus(0, 0);

	m_Event |= (LEVELREVENT | EDGEMODE);

	CLStatus s = m_pEpoll->DoEvent(this, m_Fd, EVENTCHANGE, m_Event);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLEpollEvent::OpenWEvent(), m_pEpoll->DoEvent() error", 0);
		return CLStatus(-1, 0);
	}

	m_bWriteFlag = true;
	return CLStatus(0, 0);

}

CLStatus CLEpollEvent::CloseREvent()
{
	if(!m_bReadFlag)
		return CLStatus(0, 0);

	m_Event &= ~LEVELREVENT;
	CLStatus s = m_pEpoll->DoEvent(this, m_Fd, EVENTCHANGE, m_Event);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLEpollEvent::CloseREvent(), m_pEpoll->DoEvent() error", 0);
		return CLStatus(-1, 0);
	}

	m_bReadFlag = false;
	return CLStatus(0, 0);
}

CLStatus CLEpollEvent::CloseWEvent()
{
	if(!m_bWriteFlag)
		return CLStatus(0, 0);

	m_Event &= ~LEVELWEVENT;
	CLStatus s = m_pEpoll->DoEvent(this, m_Fd, EVENTCHANGE, m_Event);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLEpollEvent::CloseWEvent(), m_pEpoll->DoEvent() error", 0);
		return CLStatus(-1, 0);
	}

	m_bWriteFlag = false;

	return CLStatus(0, 0);
}

