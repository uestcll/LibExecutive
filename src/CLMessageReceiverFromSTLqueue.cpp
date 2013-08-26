#include "CLMessageReceiverFromSTLqueue.h"
#include "CLCriticalSection.h"
#include "CLMessage.h"
#include "CLLogger.h"

CLMessageReceiverFromSTLqueue::CLMessageReceiverFromSTLqueue() : CLMessageReceiver(0)
{
}

CLMessageReceiverFromSTLqueue::~CLMessageReceiverFromSTLqueue()
{
	while(!m_MessageQueue.empty())
	{
		CLMessage *pMsg = m_MessageQueue.front();
		m_MessageQueue.pop();

		if(pMsg != NULL)
			delete pMsg;
	}
}

CLStatus CLMessageReceiverFromSTLqueue::PushMessage(CLMessage *pMessage)
{
	if(pMessage == NULL)
		return CLStatus(-1, 0);
	
	try
	{
		CLCriticalSection cs(&m_Mutex);

		m_MessageQueue.push(pMessage);

		return CLStatus(0, 0);
	}
	catch(const char* str)
	{
		CLLogger::WriteLogMsg("In CLMessageReceiverFromSTLqueue::Push(), exception arise", 0);
		delete pMessage;
		return CLStatus(-1, 0);
	}
}

CLStatus CLMessageReceiverFromSTLqueue::GetMessage(CLMessage **ppMsg)
{
	try
	{
		CLCriticalSection cs(&m_Mutex);

		if(m_MessageQueue.empty())
			return CLStatus(-1, MESSAGE_RECEIVE_NOT_ENOUGH);

		*ppMsg = m_MessageQueue.front();
		m_MessageQueue.pop();

		if(*ppMsg == NULL)
			return CLStatus(-1, MESSAGE_RECEIVE_ERROR);

		return CLStatus(0, 0);
	}
	catch(const char* str)
	{
		CLLogger::WriteLogMsg("In CLMessageReceiverFromSTLqueue::Pop(), exception arise", 0);
		return CLStatus(-1, MESSAGE_RECEIVE_ERROR);
	}
}