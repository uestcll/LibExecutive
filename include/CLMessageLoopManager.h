#ifndef CLMessageLoopManager_H
#define CLMessageLoopManager_H

#include <map>
#include <queue>
#include "CLUuid.h"
#include "CLStatus.h"

class CLMessageObserver;
class CLMessage;
class CLExecutiveInitialFinishedNotifier;

typedef CLStatus (CLMessageObserver::*CallBackForMessageLoop)(CLMessage *, CLUuid);

#define QUIT_MESSAGE_LOOP 1

struct SLExecutiveInitialParameter
{
	void *pContext;
	CLExecutiveInitialFinishedNotifier *pNotifier;
};

struct SLMessageAndSource
{
	CLMessage *pMsg;
	CLUuid ChannelUuid;
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
	
	virtual CLStatus WaitForMessage() = 0;
	virtual CLStatus DispatchMessage(SLMessageAndSource *pMsgInfo);

private:
	CLMessageLoopManager(const CLMessageLoopManager&);
	CLMessageLoopManager& operator=(const CLMessageLoopManager&);

protected:
	CLMessageObserver *m_pMessageObserver;
	std::map<unsigned long, CallBackForMessageLoop> m_MsgMappingTable;
	std::queue<SLMessageAndSource*> m_MessageContainer;
};

#endif