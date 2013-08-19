#ifndef CLMessageQueueBySTLqueue_H
#define CLMessageQueueBySTLqueue_H

#include <queue>
#include "CLStatus.h"
#include "CLMutex.h"
#include "CLEvent.h"

class CLMessage;

class CLMessageQueueBySTLqueue
{
public:
	CLMessageQueueBySTLqueue();
	virtual ~CLMessageQueueBySTLqueue();

	CLStatus PushMessage(CLMessage * pMessage);
	CLMessage* GetMessage();

private:
	CLStatus Push(CLMessage * pMessage);
	CLMessage* Pop();

private:
	CLMessageQueueBySTLqueue(const CLMessageQueueBySTLqueue&);
	CLMessageQueueBySTLqueue& operator=(const CLMessageQueueBySTLqueue&);

private:
	std::queue<CLMessage*> m_MessageQueue;
	CLMutex m_Mutex;
	CLEvent m_Event;
};

#endif