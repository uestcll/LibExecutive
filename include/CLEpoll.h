#ifndef CLEPOLL_H
#define CLEPOLL_H

#include <sys/epoll.h>
#include "CLStatus.h"

#define EPOLL_MAX_FD_SIZE 1024

class CLEpollEvent;

class CLEpoll
{
public:
	CLEpoll();
	virtual ~CLEpoll();

public:
	CLStatus Run();
	CLStatus Initialize(int maxFdSize);
	CLStatus DoEvent(CLEpollEvent *pEvent, int fd, int epollOpt, int epollEvents);


private:
	struct epoll_event *m_pEpollEvents;
	int 				m_iEpollFd;
	int 				m_iMaxEventSize;

	// CLMsgLoopManagerForEpoll *m_pMsgLoopManager;
};

#endif