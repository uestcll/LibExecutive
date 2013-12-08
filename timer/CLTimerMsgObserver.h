#ifndef CLTIMERMSGOBSERVER_H
#define CLTIMERMSGOBSERVER_H

#include "CLStatus.h"
#include "CLMessageObserver.h"

#define TIMER_PIPE_NAME "timer_server_pipe"

class CLMessageLoopManager;
class CLMessage;

class CLTimerMsgObserver : public CLMessageObserver
{
public:
	CLTimerMsgObserver();
	virtual ~CLTimerMsgObserver();

	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext);

	CLStatus On_TimerApply(CLMessage *pM);
	CLStatus On_TimerOut(CLMessage *pM);

private:
	CLMsgLoopManagerForEpoll *m_pMsgLoopManager;

private:
	CLTimerMsgObserver(const CLTimerMsgObserver&);
	CLTimerMsgObserver& operator=(const CLTimerMsgObserver&);
};

#endif