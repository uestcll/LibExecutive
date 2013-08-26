#ifndef CLMessageReceiverFromSTLqueue_H
#define CLMessageReceiverFromSTLqueue_H

#include <queue>
#include "CLMessageReceiver.h"
#include "CLMutex.h"

class CLMessage;

class CLMessageReceiverFromSTLqueue : public CLMessageReceiver
{
public:
	CLMessageReceiverFromSTLqueue();
	virtual ~CLMessageReceiverFromSTLqueue();

	virtual CLStatus GetMessage(CLMessage **ppMsg);
	CLStatus PushMessage(CLMessage * pMessage);

private:
	CLMessageReceiverFromSTLqueue(const CLMessageReceiverFromSTLqueue&);
	CLMessageReceiverFromSTLqueue& operator=(const CLMessageReceiverFromSTLqueue&);

private:
	std::queue<CLMessage*> m_MessageQueue;
	CLMutex m_Mutex;
};

#endif