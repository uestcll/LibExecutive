#ifndef CLMessageLoopManager_H
#define CLMessageLoopManager_H

#include <map>
#include "CLStatus.h"

class CLMessageObserver;
class CLMessage;
class CLExecutiveInitialFinishedNotifier;

typedef CLStatus (CLMessageObserver::*CallBackForMessageLoop)(CLMessage *);

#define QUIT_MESSAGE_LOOP 1

struct SLExecutiveInitialParameter
{
	void *pContext;
	CLExecutiveInitialFinishedNotifier *pNotifier;
};

/*
该继承体系对象应从堆中分配，不必显示调用delete
若消息处理函数在CLStatus的返回对象中，若返回QUIT_MESSAGE_LOOP返回值，将导致退出消息循环
*/
class CLMessageLoopManager
{
public:
	/*
	pMessageObserver应从堆中分配，且不用调用delete
	*/
	CLMessageLoopManager(CLMessageObserver *pMessageObserver);
	virtual ~CLMessageLoopManager();

	virtual CLStatus EnterMessageLoop(void *pContext);

public:	
	/*
	MessageObserver调用此函数，用于注册具体的消息处理函数
	*/
	virtual CLStatus Register(unsigned long lMsgID, CallBackForMessageLoop pMsgProcessFunction);

protected:
	/*
	初始化与反初始化消息循环，需要保证消息队列已经建立完毕
	*/
	virtual CLStatus Initialize() = 0;
	virtual CLStatus Uninitialize() = 0;
	
	virtual CLMessage* WaitForMessage() = 0;
	virtual CLStatus DispatchMessage(CLMessage *pMessage);

private:
	CLMessageLoopManager(const CLMessageLoopManager&);
	CLMessageLoopManager& operator=(const CLMessageLoopManager&);

protected:
	CLMessageObserver *m_pMessageObserver;
	std::map<unsigned long, CallBackForMessageLoop> m_MsgMappingTable;
};

#endif
