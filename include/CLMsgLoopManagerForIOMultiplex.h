#ifndef CLMsgLoopManagerForIOMultiplex_H
#define CLMsgLoopManagerForIOMultiplex_H

#include <sys/epoll.h>
#include <string>
#include <map>
#include "CLMessageLoopManager.h"

using namespace std;

class CLMessageQueue;
class CLMessageDeserializer;
class CLMessageQueueByNamedPipe;

#define PIPE_QUEUE_BETWEEN_PROCESS 0
#define PIPE_QUEUE_IN_PROCESS 1
#define MAX_EVENT_SIZE 20
#define MAX_FD_SIZE 256

class CLMsgLoopManagerForIOMultiplex : public CLMessageLoopManager
{
public:
	/*
	pMsgObserver¾ùÓ¦´Ó¶ÑÖÐ·ÖÅä£¬ÇÒ²»±ØÏÔÊ¾µ÷ÓÃdelete
	*/
	CLMsgLoopManagerForIOMultiplex(CLMessageObserver *pMsgObserver, const char* pstrThreadName, int PipeQueueType);
	virtual ~CLMsgLoopManagerForIOMultiplex();

	virtual CLStatus RegisterDeserializer(string strQueueName, unsigned long lMsgID, CLMessageDeserializer *pDeserializer);
	virtual CLStatus RegisterMsgQueue(string strQueueName, CLMessageQueue *pMsgQueue, bool isShort = false);
	virtual CLStatus UnRegisterMsgQueue(string strQueueName);

	CLStatus    SetNonBlocking(int fd);

protected:
	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();

	virtual CLMessage* WaitForMessage();

	CLStatus 	InitializeIOMultiplex();

private:
	CLMsgLoopManagerForIOMultiplex(const CLMsgLoopManagerForIOMultiplex&);
	CLMsgLoopManagerForIOMultiplex& operator=(const CLMsgLoopManagerForIOMultiplex&);

private:
	CLMessageQueueByNamedPipe *m_pMsgQueue;
	map<string, CLMessageQueue*> m_MsgQueueTable;

	string m_strThreadName;

	struct epoll_event m_epEvent;
   	struct epoll_event m_epEvents[MAX_EVENT_SIZE]; 
	int    m_iEpollFd;
};

#endif
