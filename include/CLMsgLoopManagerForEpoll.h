#ifndef CLMSGLOOPMANAGERFOREPOLL_H
#define CLMSGLOOPMANAGERFOREPOLL_H

#include "CLMessageLoopManager.h"
#include "CLMessageReceiver.h"

class CLEpoll;
class CLEpollEvent;

class CLMsgLoopManagerForEpoll : public CLMessageLoopManager
{
public:
	CLMsgLoopManagerForEpoll(CLMessageObserver *pMessageObserver, CLEpoll *pEpoll);
	virtual ~CLMsgLoopManagerForEpoll();

	virtual CLStatus EnterMessageLoop(void *pContext);

public:	
	virtual CLStatus Register(unsigned long lMsgID, CallBackForMessageLoop pMsgProcessFunction);

	CLStatus RecycleMsgReceiver();
	CLStatus RecycleMsgPoster();
	CLStatus NotifyReadable();

protected:
	virtual CLStatus Initialize() = 0;
	virtual CLStatus Uninitialize() = 0;
	
	virtual CLStatus WaitForMessage();
	virtual CLStatus DispatchMessage(CLMessage *pMessage);

private:
	CLMsgLoopManagerForEpoll(const CLMsgLoopManagerForEpoll&);
	CLMsgLoopManagerForEpoll& operator=(const CLMsgLoopManagerForEpoll&);

protected:
	CLEpoll *m_pEpoll;
	map<fd, CLMessageReceiver*> m_MsgReceiverMap;
};

#endif