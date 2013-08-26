#include "CLThreadCommunicationBySTLqueue.h"
#include "CLMessageReceiverFromSTLqueue.h"
#include "CLMessage.h"

CLThreadCommunicationBySTLqueue::CLThreadCommunicationBySTLqueue(CLMessageReceiverFromSTLqueue *pMsgReceiver, CLEvent *pEvent) : CLExecutiveCommunication(NULL)
{
	if(pMsgReceiver == 0)
		throw "In CLThreadCommunicationBySTLqueue::CLThreadCommunicationBySTLqueue(), pMsgReceiver error";

	if(pEvent == 0)
	{
		delete pMsgReceiver;
		throw "In CLThreadCommunicationBySTLqueue::CLThreadCommunicationBySTLqueue(), pEvent error";
	}
	
	m_pMsgReceiver = pMsgReceiver;
	m_pEvent = pEvent;
}

CLThreadCommunicationBySTLqueue::~CLThreadCommunicationBySTLqueue()
{
	delete m_pMsgReceiver;
}

CLStatus CLThreadCommunicationBySTLqueue::PostExecutiveMessage(CLMessage *pMessage)
{
	if(pMessage == NULL)
		return CLStatus(-1, 0);
	
	CLStatus s1 = m_pMsgReceiver->PushMessage(pMessage);
	if(!s1.IsSuccess())
	{
		CLLogger::WriteLogMsg("InCLThreadCommunicationBySTLqueue::PostExecutiveMessage(), m_pMsgReceiver->PushMessage error", 0);
		return s1;
	}

	CLStatus s2 = m_pEvent->Set();
	if(!s2.IsSuccess())
	{
		CLLogger::WriteLogMsg("InCLThreadCommunicationBySTLqueue::PostExecutiveMessage(), m_pEvent->Set error", 0);
		return s2;
	}

	return CLStatus(0, 0);
}