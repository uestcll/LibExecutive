#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "CLMsgLoopManagerForIOMultiplex.h"
#include "CLMessageQueueByNamedPipe.h"
#include "CLLogger.h"
#include "CLSharedMsgQueueByNamedPipe.h"
#include "CLPrivateMsgQueueByNamedPipe.h"
#include "CLExecutiveNameServer.h"
#include "CLPrivateExecutiveCommunicationByNamedPipe.h"

CLMsgLoopManagerForIOMultiplex::CLMsgLoopManagerForIOMultiplex(CLMessageObserver *pMsgObserver, const char* pstrThreadName, int PipeQueueType) : CLMessageLoopManager(pMsgObserver)
{
	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLMsgLoopManagerForPipeQueue::CLMsgLoopManagerForPipeQueue(), pstrThreadName error";

	m_strThreadName = pstrThreadName;

	if(PipeQueueType == PIPE_QUEUE_BETWEEN_PROCESS)
		m_pMsgQueue = new CLSharedMsgQueueByNamedPipe(pstrThreadName);
	else if(PipeQueueType == PIPE_QUEUE_IN_PROCESS)
		m_pMsgQueue = new CLPrivateMsgQueuebyNamedPipe(pstrThreadName);
	else
		throw "In CLMsgLoopManagerForPipeQueue::CLMsgLoopManagerForPipeQueue(), PipeQueueType error";

	CLStatus s1 = InitializeIOMultiplex();
	if (!s1.IsSuccess())
	{
		throw "In CLMsgLoopManagerForIOMultiplex::CLMsgLoopManagerForIOMultiplex(), IOMultiplex Init error";
	}
	
	CLStatus s2 = RegisterMsgQueue(m_strThreadName, m_pMsgQueue);
	if(!s2.IsSuccess())
	{
		throw "In CLMsgLoopManagerForIOMultiplex::CLMsgLoopManagerForIOMultiplex(), RegisterFdEvent for msg queue error";
	}

}

CLMsgLoopManagerForIOMultiplex::~CLMsgLoopManagerForIOMultiplex()
{
	map<string, CLMessageQueue*>::iterator it;
	for(it = m_MsgQueueTable.begin(); it != m_MsgQueueTable.end(); it++)
		delete it->second;
}

CLStatus CLMsgLoopManagerForIOMultiplex::SetNonBlocking(int fd)
{
	int opts;
    opts=fcntl(fd,F_GETFL);
    if(opts<0)
    {
 		return CLStatus(-1, 0);
    }
    opts = opts|O_NONBLOCK;
    if(fcntl(fd,F_SETFL,opts)<0)
    {
		return CLStatus(-1, 0);
    }

	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForIOMultiplex::RegisterMsgQueue(string strQueueName, CLMessageQueue *pMsgQueue, bool isShort)
{
	int fd;
	fd = pMsgQueue->m_ciFd;
	
/*	if(!SetNonBlocking(fd).IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForIOMultiplex::RegisterFdEvent(), set non blocking error", 0);
	}
*/
	m_epEvent.data.fd = fd;
	m_epEvent.events = EPOLLIN | EPOLLET;
	m_epEvent.data.ptr = pMsgQueue;

	if(isShort)
		m_epEvent.events |= EPOLLONESHOT;

	if(0 != epoll_ctl(m_iEpollFd, EPOLL_CTL_ADD, fd, &m_epEvent))
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForIOMultiplex::RegisterFdEvent(), epoll add error", 0);
		return CLStatus(-1, 0);
	}	
	m_MsgQueueTable[strQueueName] = pMsgQueue;

	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForIOMultiplex::UnRegisterMsgQueue(string strQueueName)
{
	if(m_MsgQueueTable.find(strQueueName) == m_MsgQueueTable.end())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForIOMultiplex::UnRegisterFdEvent(), not exist", 0);
		return CLStatus(-1, 0);
	}	
	int fd;
	fd = m_MsgQueueTable[strQueueName]->m_ciFd;

	if(0 != epoll_ctl(m_iEpollFd, EPOLL_CTL_DEL, fd, &m_epEvent))
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForIOMultiplex::UnRegisterFdEvent(), epoll del error", 0);
		return CLStatus(-1, 0);
	}
	m_MsgQueueTable.erase(strQueueName);
}

CLStatus CLMsgLoopManagerForIOMultiplex::InitializeIOMultiplex()
{
	m_iEpollFd = epoll_create(MAX_FD_SIZE);

	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForIOMultiplex::Initialize()
{
	CLPrivateMsgQueuebyNamedPipe *pQueue = dynamic_cast<CLPrivateMsgQueuebyNamedPipe *>(m_pMsgQueue);
	if(pQueue == 0)
		return CLStatus(0, 0);

	CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
	if(pNameServer == 0)
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForPipeQueue::Initialize(), CLExecutiveNameServer::GetInstance error", 0);
		return CLStatus(-1, 0);
	}

	CLStatus s = pNameServer->Register(m_strThreadName.c_str(), new CLPrivateExecutiveCommunicationByNamedPipe(m_strThreadName.c_str()));
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForPipeQueue::Initialize(), pNameServer->Register error", 0);
		return CLStatus(-1, 0);
	}

	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForIOMultiplex::Uninitialize()
{
	CLPrivateMsgQueuebyNamedPipe *pQueue = dynamic_cast<CLPrivateMsgQueuebyNamedPipe *>(m_pMsgQueue);
	if(pQueue == 0)
		return CLStatus(0, 0);

	CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
	if(pNameServer == 0)
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForPipeQueue::Uninitialize(), CLExecutiveNameServer::GetInstance error", 0);
		return CLStatus(-1, 0);
	}

	return pNameServer->ReleaseCommunicationPtr(m_strThreadName.c_str());
}

CLMessage* CLMsgLoopManagerForIOMultiplex::WaitForMessage()
{
	//return m_pMsgQueue->GetMessage();
	int nfds, i;
	int fd;
	nfds = epoll_wait(m_iEpollFd, m_epEvents, MAX_EVENT_SIZE, -1);
	for(i = 0; i < nfds; i++)
	{
		if(m_epEvents[i].events & EPOLLIN)
		{
			CLMessageQueue *pQueue = static_cast<CLMessageQueue*>(m_epEvents[i].data.ptr);

			return pQueue->GetMessage();
		}
	}
	
}

CLStatus CLMsgLoopManagerForIOMultiplex::RegisterDeserializer(string strQueueName, unsigned long lMsgID, CLMessageDeserializer *pDeserializer)
{
	CLMessageQueue *pMsgQueue = m_MsgQueueTable[strQueueName];
	CLSharedMsgQueueByNamedPipe *pQueue = dynamic_cast<CLSharedMsgQueueByNamedPipe *>(pMsgQueue);
	if(pQueue != 0)
		return pQueue->RegisterDeserializer(lMsgID, pDeserializer);
	else
		return CLStatus(-1, 0);
}
