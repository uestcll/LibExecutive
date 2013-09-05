#ifndef CLThreadCommunicationBySTLqueue_H
#define CLThreadCommunicationBySTLqueue_H

#include "CLExecutiveCommunication.h"
#include "CLStatus.h"
#include "CLEvent.h"

class CLMessage;
class CLMessageReceiverBySTLqueue;

class CLThreadCommunicationBySTLqueue : public CLExecutiveCommunication
{
public:
	/*
	pMsgQueue±ØÐë´Ó¶ÑÖÐ·ÖÅä£¬ÇÒ²»±ØÏÔÊ¾µ÷ÓÃdelete
	*/
	CLThreadCommunicationBySTLqueue(CLMessageReceiverBySTLqueue *pMsgReceiver, CLEvent *pEvent);
	virtual ~CLThreadCommunicationBySTLqueue();

	virtual CLStatus PostExecutiveMessage(CLMessage *pMessage);

private:
	CLThreadCommunicationBySTLqueue(const CLThreadCommunicationBySTLqueue&);
	CLThreadCommunicationBySTLqueue& operator=(const CLThreadCommunicationBySTLqueue&);

private:
	CLMessageReceiverBySTLqueue *m_pMsgReceiver;
	CLEvent *m_pEvent;
};

#endif