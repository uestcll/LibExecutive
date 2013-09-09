#include "CLMessageReceiver.h"

CLMessageReceiver::CLMessageReceiver(CLProtoParser *pProtoParser) : m_ciFd(m_Fd)
{
	m_pProtoParser = pProtoParser;
	m_Fd = 0;
}

CLMessageReceiver::~CLMessageReceiver()
{
	if(m_pProtoParser != NULL)
	{
		delete m_pProtoParser;
		m_pProtoParser = NULL;
	}

	while(true)
	{
		CLMessage *pMsg = GetMessage();
		if(pMsg == 0)
			break;

		delete pMsg;
	}
}

CLMessage* CLMessageReceiver::GetMessage()
{
	try
	{
		while(1)
		{
			CLMessage* pMsg = GetMessageFromChannel();
			if(pMsg == NULL)
			{
				break;
			}
			{
				CLCriticalSection cs(&m_Mutex);
				m_MessageQueue.push(pMessage);
			}
		}

		{
			CLCriticalSection cs(&m_Mutex);

			if(m_MessageQueue.empty())
				return 0;

			CLMessage *p = m_MessageQueue.front();
			m_MessageQueue.pop();
			return p;
		}
	}
	catch(const char* str)
	{
		CLLogger::WriteLogMsg("In CLMessageReceiver::GetMessage(), exception arise", 0);
		return 0;
	}
}