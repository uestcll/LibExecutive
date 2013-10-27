#ifndef CLThreadForMsgLoop_H
#define CLThreadForMsgLoop_H

#include "CLStatus.h"
#include "CLNonThreadForMsgLoop.h"

class CLMessageObserver;
class CLThread;
class CLMessageSerializer;
class CLMessageDeserializer;
class CLMultiMsgSerializer;
class CLMultiMsgDeserializer;

class CLThreadForMsgLoop
{
public:
	CLThreadForMsgLoop(CLMessageObserver *pMsgObserver, const char *pstrThreadName, bool bWaitForDeath = false, int ExecutiveType = EXECUTIVE_IN_PROCESS_USE_STL_QUEUE);
	virtual ~CLThreadForMsgLoop();

	CLStatus Run(void *pContext);
	CLStatus RegisterSerializer(unsigned long lMsgID, CLMessageSerializer *pSerializer);
	CLStatus RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer);

private:
	CLThreadForMsgLoop(const CLThreadForMsgLoop&);
	CLThreadForMsgLoop& operator=(const CLThreadForMsgLoop&);

private:
	CLThread *m_pThread;
	bool m_bWaitForDeath;
	CLMultiMsgSerializer *m_pSerializer;
	CLMultiMsgDeserializer *m_pDeserializer;
};

#endif