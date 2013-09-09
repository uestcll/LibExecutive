#ifndef CLMessageReceiverBySTLqueue_H
#define CLMessageReceiverBySTLqueue_H

#include "CLStatus.h"
#include "CLMessageReceiver.h"


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

	virtual CLMessage* GetMessageFromChannel();

private:
	CLMessageReceiverBySTLqueue(const CLMessageQueueBySTLqueue&);
	CLMessageReceiverBySTLqueue& operator=(const CLMessageQueueBySTLqueue&);

};

#endif