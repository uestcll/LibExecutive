#ifndef CLMsgLoopManagerForPipeQueue_H
#define CLMsgLoopManagerForPipeQueue_H

#include <string>
#include "CLMessageLoopManager.h"

using namespace std;

class CLMessageQueueByNamedPipe;
class CLMessageDeserializer;

#define PIPE_QUEUE_BETWEEN_PROCESS 0
#define PIPE_QUEUE_IN_PROCESS 1

class CLMsgLoopManagerForPipeQueue : public CLMessageLoopManager
{
public:
	/*
	pMsgObserver均应从堆中分配，且不必显示调用delete
	*/
	CLMsgLoopManagerForPipeQueue(CLMessageObserver *pMsgObserver, const char* pstrThreadName, int PipeQueueType);
	virtual ~CLMsgLoopManagerForPipeQueue();

	CLStatus RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer);

protected:
	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();

	virtual CLMessage* WaitForMessage();

private:
	CLMsgLoopManagerForPipeQueue(const CLMsgLoopManagerForPipeQueue&);
	CLMsgLoopManagerForPipeQueue& operator=(const CLMsgLoopManagerForPipeQueue&);

private:
	CLMessageQueueByNamedPipe *m_pMsgQueue;
	string m_strThreadName;
};

#endif