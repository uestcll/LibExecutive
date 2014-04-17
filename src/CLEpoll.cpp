#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "CLEpoll.h"
#include "CLLogger.h"
#include "CLMsgLoopManagerForEpoll.h"
#include "CLEpollEvent.h"
#include "CLMessagePoster.h"

#define DEFAULT_EPOLLWAIT_TIME 5

CLEpoll::CLEpoll()
{
	m_pEpollEvents = NULL;
	m_iEpollFd = -1;
	m_iMaxEventSize = 0;
}

CLEpoll::~CLEpoll()
{
	close(m_iEpollFd);
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

CLStatus CLEpoll::DoEvent(CLEpollEvent *pEvent, int fd, int epollOpt, unsigned int epollEvents)
{
	struct epoll_event ev;
	memset(&ev, 0, sizeof(struct epoll_event));
	ev.data.ptr = pEvent;
	ev.events = epollEvents;
	//ev.data.fd = fd;

	if(0 > epoll_ctl(m_iEpollFd, epollOpt, fd, &ev))
	{
		CLLogger::WriteLogMsg("In CLEpoll::DoEvent(), epoll_ctl() error", errno);
		return CLStatus(-1, errno);
	}

	return CLStatus(0, 0);
}


CLStatus CLEpoll::Run()
{
	int nfds;
	CLEpollEvent *pEvent = NULL;

	while(1)
	{
		if( (nfds = epoll_wait(m_iEpollFd, m_pEpollEvents, m_iMaxEventSize, DEFAULT_EPOLLWAIT_TIME)) < 0)
		{
			if(EINTR == errno)
				continue;
			else
				CLLogger::WriteLogMsg("In CLEpoll::Run(), epoll_wait error", errno);
		}

		for(int i = 0; i < nfds; ++i)
		{
			pEvent = (CLEpollEvent *)m_pEpollEvents[i].data.ptr;
			if(pEvent == NULL)
				continue;

			
            if ( m_pEpollEvents[i].events & EPOLLOUT )
            {
            	//CLDelayedDataPoster *pDataPoster = (CLDelayedDataPoster *)m_pEpollEvents[i].data.ptr;
            	// if(pDataPoster == NULL)
            	// {
            	// 	CLLogger::WriteLogMsg("In CLEpoll::Run(), pDataPoster == NULL", 0);
            	// 	continue;
             //   	}
             //   	CLStatus s = pDataPoster->SendData();
             //   	if(!s.IsSuccess())
             //   	{
             //   		CLLogger::WriteLogMsg("In CLEpoll::Run(), pDataPoster->SendData() error", 0);
             //   	}
            	/*CLMessagePoster *pMsgPoster = (CLMessagePoster *)(pEvent->GetHandler());
            	if(pMsgPoster)
            	{
            		CLStatus s = pMsgPoster->NotifyWritable();
            		if(!s.IsSuccess())
            		{
            			//pMsgPoster->UnRegister();
            			CLLogger::WriteLogMsg("In CLEpoll::Run(), pMsgPoster->NotifyWritable() error", 0);
            		}
            	}
            	else
            	{
            		CLLogger::WriteLogMsg("In CLEpoll::Run(), pMsgPoster is NULL", 0);
            	} */           	

               	continue;
            }
            if ( m_pEpollEvents[i].events & EPOLLIN )
            {
            	CLMsgLoopManagerForEpoll *pMsgLoopManager = (CLMsgLoopManagerForEpoll *)(pEvent->GetHandler());
            	if(pMsgLoopManager)
            	{
            		CLStatus s = pMsgLoopManager->NotifyReadable(pEvent->GetFd());
            		if(!s.IsSuccess())
            		{
            			//pMsgLoopManager->RecycleReceiver();
            			CLLogger::WriteLogMsg("In CLEpoll::Run(), pMsgLoopManager->NotifyReadable() error", 0);
            		}
            	}
            	else
            	{
            		CLLogger::WriteLogMsg("In CLEpoll::Run(), pMsgLoopManager is NULL", 0);
            	}
                continue;
            }
            
            if ( m_pEpollEvents[i].events & EPOLLERR || m_pEpollEvents[i].events & EPOLLHUP )
            {
               continue;
            }
		}
	}

	return CLStatus(0, 0);
}

