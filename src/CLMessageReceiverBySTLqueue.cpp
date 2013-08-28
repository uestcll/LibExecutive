#include "CLMessageReceiverBySTLqueue.h"
#include "CLCriticalSection.h"
#include "CLMessage.h"
#include "CLLogger.h"

CLMessageReceiverBySTLqueue::CLMessageReceiverBySTLqueue()
{
}

CLMessageReceiverBySTLqueue::~CLMessageReceiverBySTLqueue()
{
	while(true)
	{
		CLMessage *pMsg = GetMessage();
		if(pMsg == 0)
			break;

		delete pMsg;
	}
}

CLStatus CLMessageReceiverBySTLqueue::PushMessage(CLMessage * pMessage)
{
	try
	{
		CLCriticalSection cs(&m_Mutex);

		m_MessageQueue.push(pMessage);
	}
	catch(const char* str)
	{
		CLLogger::WriteLogMsg("In CLMessageReceiverBySTLqueue::PushMessage(), exception arise", 0);
		delete pMessage;
		return CLStatus(-1, 0);
	}

	return CLStatus(0, 0);
}

CLMessage* CLMessageReceiverBySTLqueue::GetMessage()
{
	try
	{
		CLCriticalSection cs(&m_Mutex);

		if(m_MessageQueue.empty())
			return 0;

		CLMessage *p = m_MessageQueue.front();
		m_MessageQueue.pop();
		return p;
	}
	catch(const char* str)
	{
		CLLogger::WriteLogMsg("In CLMessageReceiverBySTLqueue::GetMessage(), exception arise", 0);
		return 0;
	}
}
