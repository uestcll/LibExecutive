#ifndef CLThreadCommunicationBySTLqueue_H
#define CLThreadCommunicationBySTLqueue_H

#include "CLExecutiveCommunication.h"
#include "CLStatus.h"

class CLMessage;
class CLMessageReceiverFromSTLqueue;
class CLEvent;

class CLThreadCommunicationBySTLqueue : public CLExecutiveCommunication
{
public:
	CLThreadCommunicationBySTLqueue(CLMessageReceiverFromSTLqueue *pMsgReceiver, CLEvent *pEvent);
	virtual ~CLThreadCommunicationBySTLqueue();

	virtual CLStatus PostExecutiveMessage(CLMessage *pMessage);

private:
	CLThreadCommunicationBySTLqueue(const CLThreadCommunicationBySTLqueue&);
	CLThreadCommunicationBySTLqueue& operator=(const CLThreadCommunicationBySTLqueue&);

private:
	CLMessageReceiverFromSTLqueue *m_pMsgReceiver;
	CLEvent *m_pEvent;
};

#endif