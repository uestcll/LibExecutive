#include "CLMessageReceiver.h"
#include "CLStatus.h"
#include "CLLogger.h"
#include "CLIOVector.h"

using namespace std;

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
		m_pIOBufVec = new CLIOVector();
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
	CLStatus s = m_pDataReceiver->GetDataIOVec(m_pIOBufVec);

	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessageReceiver::GetMessage(), m_pDataReceiver->GetDataIOVec() error", 0);
		return PopMessage();
	}

	vector<SLSerializedMsg> pSerializedMsgVec;//存储协议解析切割完成，反序列化之前的一个个的协议。
	CLIOVector *pRestBufVec;

	m_pProtoParser->Decapsulate(m_pIOBufVec, pSerializedMsgVec, &pRestBufVec);
	
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

CLMessage* CLMessageReceiver::PopMessage()
{
	CLCriticalSection cs(&m_Mutex);

	if(m_MessageQueue.empty())
		return 0;

	CLMessage *p = m_MessageQueue.front();
	m_MessageQueue.pop();
	return p;	
}