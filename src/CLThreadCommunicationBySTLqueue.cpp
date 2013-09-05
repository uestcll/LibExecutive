#include "CLThreadCommunicationBySTLqueue.h"
#include "CLMessageReceiverBySTLqueue.h"
#include "CLMessage.h"

CLThreadCommunicationBySTLqueue::CLThreadCommunicationBySTLqueue(CLMessageReceiverBySTLqueue *pMsgReceiver, CLEvent *pEvent)
{
	if(pMsgReceiver == 0 || pEvent == 0)
		throw "In CLThreadCommunicationBySTLqueue::CLThreadCommunicationBySTLqueue(), pMsgReceiver or pEvent error";
	
	m_pMsgReceiver = pMsgReceiver;
	m_pEvent = pEvent;
}

CLThreadCommunicationBySTLqueue::~CLThreadCommunicationBySTLqueue()
{
	if(m_pMsgReceiver != NULL)
	{
		delete m_pMsgReceiver;
		m_pMsgReceiver = NULL;
	}
}

CLStatus CLThreadCommunicationBySTLqueue::PostExecutiveMessage(CLMessage *pMessage)
{
	if(pMessage == NULL)
		return CLStatus(-1, 0);
	
	CLStatus s = m_pMsgReceiver->PushMessage(pMessage);

	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLThreadCommunicationBySTLqueue::PostExecutiveMessage(), PushMsg error", 0);
		return CLStatus(-1, 0);
	}

	CLStatus s2 = m_pEvent->Set();
	if(!s2.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLThreadCommunicationBySTLqueue::PostExecutiveMessage(), m_Event.Set error", 0);
		return CLStatus(-1, 0);
	}
	
	return CLStatus(0, 0);
}