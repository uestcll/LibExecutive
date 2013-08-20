#ifndef CLThreadForMsgLoop_H
#define CLThreadForMsgLoop_H

#include "CLStatus.h"
#include "CLNonThreadForMsgLoop.h"

class CLMessageObserver;
class CLThread;
class CLMsgLoopManagerForPipeQueue;
class CLMessageDeserializer;

class CLThreadForMsgLoop
{
public:
	CLThreadForMsgLoop(CLMessageObserver *pMsgObserver, const char *pstrThreadName, bool bWaitForDeath = false, int ExecutiveType = EXECUTIVE_IN_PROCESS_USE_STL_QUEUE);
	virtual ~CLThreadForMsgLoop();

	CLStatus RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer);
	CLStatus Run(void *pContext);

private:
	CLThreadForMsgLoop(const CLThreadForMsgLoop&);
	CLThreadForMsgLoop& operator=(const CLThreadForMsgLoop&);

private:
	CLThread *m_pThread;
	bool m_bWaitForDeath;

	CLMsgLoopManagerForPipeQueue *m_pPipeQueue;
};

#endif