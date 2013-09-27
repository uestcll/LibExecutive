#include "CLMessageReceiver.h"
#include "CLStatus.h"
#include "CLLogger.h"
#include "CLIOVector.h"
#include "CLBuffer.h"
// iovector 提供返回连续空间的长度值 CLBuffer返回一个根据长度和索引iovector 协议解析器返回的不变，其中每个iovec的len都有。所以好反序列化
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
		m_pDataBuffer = new CLBuffer();
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
	CLStatus s1 = m_pDataReceiver->GetData(m_pDataBuffer);

	if(!s1.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessageReceiver::GetMessage(), m_pDataReceiver->GetData() error", 0);
		return PopMessage();
	}

	vector<SLSerializedMsg> pSerializedMsgVec;//存储协议解析切割完成，反序列化之前的一个个的协议。
	CLIOVector *pRestBufVec;

	CLStatus s2 = m_pProtoParser->Decapsulate(m_pDataBuffer, pSerialBufferec);
	if(!s2.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessageReceiver::GetMessage(0, m_pProtoParser->Decapsulate(), error", 0);
		return PopMessage();
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