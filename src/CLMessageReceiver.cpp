#include <memory.h>
#include "CLMessageReceiver.h"
#include "CLMessageDeserializer.h"
#include "CLProtocolDecapsulator.h"
#include "CLDataReceiver.h"
#include "CLIOVectors.h"
#include "CLLogger.h"
#include "ErrorCode.h"
#include "CLBufferManager.h"

using namespace std;

CLMessageReceiver::CLMessageReceiver(CLBufferManager *pBufferManager, CLDataReceiver *pDataReceiver, CLMessageDeserializer *pMsgDeserializer, CLProtocolDecapsulator *pProtocolDecapsulator)
{
	try
	{
		if(pBufferManager == 0)
			throw "In CLMessageReceiver::CLMessageReceiver(), pBufferManager == 0 error";

		if(pDataReceiver == 0)
			throw "In CLMessageReceiver::CLMessageReceiver(), pDataReceiver == 0 error";

		if(pMsgDeserializer == 0)
			throw "In CLMessageReceiver::CLMessageReceiver(), pMsgDeserializer == 0 error";

		m_pDataReceiver = pDataReceiver;
		m_pProtocolDecapsulator = pProtocolDecapsulator;
		m_pMsgDeserializer = pMsgDeserializer;
		m_pBufferManager = pBufferManager;
	}
	catch(const char *s)
	{
		if(pBufferManager)
			delete pBufferManager;

		if(pDataReceiver)
			delete pDataReceiver;

		if(pProtocolDecapsulator)
			delete pProtocolDecapsulator;

		if(pMsgDeserializer)
			delete pMsgDeserializer;

		throw s;
	}
}

CLMessageReceiver::~CLMessageReceiver()
{
	if(m_pBufferManager)
		m_pBufferManager->SetDestroyFlag();

	if(m_pMsgDeserializer)
		delete m_pMsgDeserializer;

	if(m_pProtocolDecapsulator)
		delete m_pProtocolDecapsulator;

	if(m_pDataReceiver)
		delete m_pDataReceiver;
}

CLStatus CLMessageReceiver::GetMessage(std::queue<CLMessage*>& qMsgContainer)
{
	CLIOVectors ReceiveIOVec;
	m_pBufferManager->GetEmptyIOVector(ReceiveIOVec);

	long Context = 0;
	CLStatus s1 = m_pDataReceiver->GetData(ReceiveIOVec, &Context);
	if(!s1.IsSuccess())
	{
		if(s1.m_clReturnCode != RECEIVED_ZERO_BYTE)
			CLLogger::WriteLogMsg("In CLMessageReceiver::GetMessage(), m_pDataReceiver->GetData error", 0);

		return s1;
	}

	CLIOVectors DataIOVec;
	m_pBufferManager->GetPartialDataIOVector(DataIOVec);

	unsigned int length = DataIOVec.Size();
	length = length + s1.m_clReturnCode;
	
	DataIOVec.PushBackIOVector(ReceiveIOVec, IOVECTOR_NON_DELETE);

	if(m_pProtocolDecapsulator)
	{
		vector<CLIOVectors *> vSerializedMsgs;
		CLIOVectors IOVectorsForPartialData;

		try
		{
			CLStatus s2 = m_pProtocolDecapsulator->Decapsulate(DataIOVec, length, vSerializedMsgs, IOVectorsForPartialData, &Context);
			if(!s2.IsSuccess())
			{
				CLLogger::WriteLogMsg("In CLMessageReceiver::GetMessage(), m_pProtocolDecapsulator->Decapsulate error", 0);
				throw s2;
			}

			if(IOVectorsForPartialData.Size() != 0)
				m_pBufferManager->SetPartialDataIOVector(IOVectorsForPartialData);

			for(int i = 0; i < vSerializedMsgs.size(); i++)
			{
				CLMessage *pMsg = 0;
				CLStatus s3 = m_pMsgDeserializer->Deserialize(*(vSerializedMsgs[i]), &pMsg, *m_pBufferManager);
				if(!s3.IsSuccess() || (pMsg == 0))
				{
					CLLogger::WriteLogMsg("In CLMessageReceiver::GetMessage(), m_pMsgDeserializer->Deserialize or pMsg == 0 error", 0);
					throw s3;
				}

				qMsgContainer.push(pMsg);
			}

			throw CLStatus(0, 0);
		}
		catch(CLStatus& s)
		{
			for(int i = 0; i < vSerializedMsgs.size(); i++)
				delete vSerializedMsgs[i];

			return s;
		}
	}

	CLMessage *pMsg1 = 0;
	CLStatus s4 = m_pMsgDeserializer->Deserialize(DataIOVec, &pMsg1, *m_pBufferManager);
	if(!s4.IsSuccess() || (pMsg1 == 0))
	{
		CLLogger::WriteLogMsg("In CLMessageReceiver::GetMessage(), m_pMsgDeserializer->Deserialize or pMsg1 == 0 error", 0);
		return s4;
	}

	qMsgContainer.push(pMsg1);

	return CLStatus(0, 0);
}