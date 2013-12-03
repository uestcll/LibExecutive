#ifndef CLMSGLOOPMANAGERFOREPOLL_H
#define CLMSGLOOPMANAGERFOREPOLL_H

#include "CLMessageLoopManager.h"
#include "CLMessageReceiver.h"

class CLEpoll;
class CLEpollEvent;

#define EPOLL_MAX_FD_SIZE 1024

class CLMsgLoopManagerForEpoll : public CLMessageLoopManager
{
public:
	CLMsgLoopManagerForEpoll(CLMessageObserver *pMessageObserver, CLEpoll *pEpoll);
	virtual ~CLMsgLoopManagerForEpoll();

	virtual CLStatus EnterMessageLoop(void *pContext);

public:	
	virtual CLStatus Register(unsigned long lMsgID, CallBackForMessageLoop pMsgProcessFunction);
	virtual CLStatus RegisterMsgReceiver(CLMessageReceiver *pReceiver);

	CLStatus RecycleMsgReceiver();
	CLStatus RecycleMsgPoster();
	CLStatus NotifyReadable(int fd);

protected:
	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();
	
	virtual CLStatus WaitForMessage();

private:
	CLMsgLoopManagerForEpoll(const CLMsgLoopManagerForEpoll&);
	CLMsgLoopManagerForEpoll& operator=(const CLMsgLoopManagerForEpoll&);

protected:
	CLEpoll *m_pEpoll;
	CLMessageReceiver* m_pMsgReceiver;
	map<fd, CLMessageReceiver*> m_MsgReceiverMap;//!!!!
	map<fd, CLEpollEvent*> m_EpollEventMap;
};

#endif