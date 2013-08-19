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

class CLMessageLoopManager
{
public:
	CLMessageLoopManager(CLMessageObserver *pMessageObserver);
	virtual ~CLMessageLoopManager();

	virtual CLStatus EnterMessageLoop(void *pContext);

public:	
	virtual CLStatus Register(unsigned long lMsgID, CallBackForMessageLoop pMsgProcessFunction);

protected:
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