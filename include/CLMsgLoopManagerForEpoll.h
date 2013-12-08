#ifndef CLMSGLOOPMANAGERFOREPOLL_H
#define CLMSGLOOPMANAGERFOREPOLL_H
#include <string>

#include "CLMessageLoopManager.h"
#include "CLMessageReceiver.h"

class CLEpoll;
class CLEpollEvent;
class CLMessageDeserializer;
class CLMultiMsgDeserializer;
class CLProtoParser;
class CLPointerMsgDeserializer;

using namespace std;

class CLMsgLoopManagerForEpoll : public CLMessageLoopManager
{
public:
	CLMsgLoopManagerForEpoll(CLMessageObserver *pMessageObserver, CLEpoll *pEpoll, CLMultiMsgDeserializer *pMsgDeserializer = NULL);
	virtual ~CLMsgLoopManagerForEpoll();

	virtual CLStatus EnterMessageLoop(void *pContext);

	CLStatus RegisterPipeReceiver(string strPipeName, int pipeType);
	CLStatus RegisterTimerReceiver(struct itimerspec& sTimerValue, string pstrRemoteName, const int& ID);
	// CLStatus RegisterSocketReceiver(CLSocket );

	CLStatus UnRegisterMsgReceiver(int fd);
	CLStatus RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer);
	// CLStatus RecycleMsgReceiver();
	// CLStatus RecycleMsgPoster();
	CLStatus NotifyReadable(int fd);

public:	
	virtual CLStatus Register(unsigned long lMsgID, CallBackForMessageLoop pMsgProcessFunction);
	
protected:
	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();
	
	virtual CLStatus WaitForMessage();

	virtual CLStatus RegisterMsgReceiver(CLMessageReceiver *pReceiver);

private:
	CLMsgLoopManagerForEpoll(const CLMsgLoopManagerForEpoll&);
	CLMsgLoopManagerForEpoll& operator=(const CLMsgLoopManagerForEpoll&);

private:
	CLEpoll *m_pEpoll;
	CLMessageReceiver* m_pMsgReceiver;
	CLMultiMsgDeserializer *m_pMultiMsgDeserializer;
	CLPointerMsgDeserializer *m_pPointerMsgDeserializer;
	map<int, CLMessageReceiver*> m_MsgReceiverMap;//!!!!
	map<int, CLEpollEvent*> m_EpollEventMap;
};

#endif