#include "CLThreadCommunicationBySTLqueue.h"
#include "CLMessageQueueBySTLqueue.h"
#include "CLMessage.h"

CLThreadCommunicationBySTLqueue::CLThreadCommunicationBySTLqueue(CLMessageQueueBySTLqueue*pMsgQueue)
{
	if(pMsgQueue == 0)
		throw "In CLThreadCommunicationBySTLqueue::CLThreadCommunicationBySTLqueue(), pMsgQueue error";
	
	m_pMsgQueue = pMsgQueue;
}

CLThreadCommunicationBySTLqueue::~CLThreadCommunicationBySTLqueue()
{
	delete m_pMsgQueue;
}

CLStatus CLThreadCommunicationBySTLqueue::PostExecutiveMessage(CLMessage *pMessage)
{
	if(pMessage == NULL)
		return CLStatus(-1, 0);
	
	return m_pMsgQueue->PushMessage(pMessage);
}