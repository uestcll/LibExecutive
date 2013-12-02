#ifndef CLEPOLL_H
#define CLEPOLL_H

#include <sys/epoll.h>
#include "CLStatus.h"

class CLMsgLoopManagerForEpoll;

class CLEpoll
{
public:
	CLEpoll();
	virtual ~CLEpoll();

public:
	CLStatus Run();
	CLStatus Initialize(int maxFdSize, CLMsgLoopManagerForEpoll *pMsgLoopManager = NULL);


private:
	struct epoll_event *m_pEpollEvents;
	int 				m_iEpollFd;
	int 				m_iMaxEventSize;

	CLMsgLoopManagerForEpoll *m_pMsgLoopManager;
};

#endif