#include "CLMessageReceiver.h"

CLMessageReceiver::CLMessageReceiver(CLProtoParser *pProtoParser, CLDataReceiver *pDataReceiver)
{
	try
	{
		if(pDataReceiver == NULL)
		{
			throw "In CLMessageReceiver::CLMessageReceiver(), pDataReceiver = NULL";
		}
		if(pMsgDeserializerManager == NULL)
		{
			throw "In CLMessageReceiver::CLMessageReceiver(), pMsgDeserializerManager = NULL";
		}

		m_pProtoParser = pProtoParser;
		m_pDataReceiver = pDataReceiver;
		m_pMsgDeserializerManager = pMsgDeserializerManager;
	}
	catch(const char* s)
	{

		if(m_pProtoParser != NULL)
		{
			delete m_pProtoParser;
			m_pProtoParser = NULL;
		}
		if(m_pDataReceiver != NULL)
		{
			delete m_pDataReceiver;
			m_pDataReceiver = NULL;
		}
		if(m_pMsgDeserializerManager != NULL)
		{
			delete m_pMsgDeserializerManager;
			m_pMsgDeserializerManager = NULL;
		}

		throw s;

	}
}

CLMessageReceiver::~CLMessageReceiver()
{
	
	if(m_pProtoParser != NULL)
	{
		delete m_pProtoParser;
		m_pProtoParser = NULL;
	}
	if(m_pDataReceiver != NULL)
	{
		delete m_pDataReceiver;
		m_pDataReceiver = NULL;
	}
	if(m_pMsgDeserializerManager != NULL)
	{
		delete m_pMsgDeserializerManager;
		m_pMsgDeserializerManager = NULL;
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