#ifndef CLMessageReceiverBySTLqueue_H
#define CLMessageReceiverBySTLqueue_H

#include <queue>
#include "CLStatus.h"
#include "CLMutex.h"
#include "CLEvent.h"
#include "CLMessageReceiver.h"

class CLMessage;

/*
该类是线程安全的
若要和CLMsgLoopManagerForMsgQueue 结合使用，则该类对象需要从堆中分配，且不用调用delete
*/
class CLMessageReceiverBySTLqueue : public CLMessageReceiver
{
public:
	/*
	构造函数和析构函数出错时，会抛出字符串类型的异常
	*/
	CLMessageReceiverBySTLqueue();
	virtual ~CLMessageReceiverBySTLqueue();

public:
	CLStatus PushMessage(CLMessage * pMessage);
	virtual CLMessage* GetMessage();

private:
	CLMessageReceiverBySTLqueue(const CLMessageQueueBySTLqueue&);
	CLMessageReceiverBySTLqueue& operator=(const CLMessageQueueBySTLqueue&);

private:
	std::queue<CLMessage*> m_MessageQueue;
	CLMutex m_Mutex;
};

#endif