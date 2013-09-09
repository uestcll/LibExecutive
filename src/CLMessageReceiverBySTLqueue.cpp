#include "CLMessageReceiverBySTLqueue.h"
#include "CLCriticalSection.h"
#include "CLMessage.h"
#include "CLLogger.h"

CLMessageReceiverBySTLqueue::CLMessageReceiverBySTLqueue() : CLMessageReceiver(NULL)
{
}

CLMessageReceiverBySTLqueue::~CLMessageReceiverBySTLqueue()
{
	
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

CLMessage* CLMessageReceiverBySTLqueue::ReadDataToMessageQueue()
{
	return NULL;
}
