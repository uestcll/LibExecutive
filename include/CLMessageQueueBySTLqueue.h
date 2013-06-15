#ifndef CLMessageQueueBySTLqueue_H
#define CLMessageQueueBySTLqueue_H

#include <queue>
#include "CLStatus.h"
#include "CLMutex.h"
#include "CLEvent.h"

class CLMessage;

/*
该类是线程安全的
若要和CLMsgLoopManagerForMsgQueue 结合使用，则该类对象需要从堆中分配，且不用调用delete
*/
class CLMessageQueueBySTLqueue
{
public:
	/*
	构造函数和析构函数出错时，会抛出字符串类型的异常
	*/
	CLMessageQueueBySTLqueue();
	virtual ~CLMessageQueueBySTLqueue();

public:
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