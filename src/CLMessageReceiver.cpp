#include <memory.h>
#include "CLMessageReceiver.h"
#include "CLMessageDeserializer.h"
#include "CLProtocolDecapsulator.h"
#include "CLDataReceiver.h"
#include "CLIOVectors.h"
#include "CLLogger.h"
#include "ErrorCode.h"

using namespace std;

CLMessageReceiver::CLMessageReceiver(CLDataReceiver *pDataReceiver, CLMessageDeserializer *pMsgDeserializer, CLProtocolDecapsulator *pProtocolDecapsulator)
{
	try
	{
		if(pDataReceiver == 0)
			throw "In CLMessageReceiver::CLMessageReceiver(), pDataReceiver == 0 error";

		if(pMsgDeserializer == 0)
			throw "In CLMessageReceiver::CLMessageReceiver(), pMsgDeserializer == 0 error";

		m_pDataReceiver = pDataReceiver;
		m_pProtocolDecapsulator = pProtocolDecapsulator;
		m_pMsgDeserializer = pMsgDeserializer;
		m_pIOVecsForPartialData = new CLIOVectors(true);
	}
	catch(const char *s)
	{
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
	if(m_pMsgDeserializer)
		delete m_pMsgDeserializer;

	if(m_pProtocolDecapsulator)
		delete m_pProtocolDecapsulator;

	if(m_pDataReceiver)
		delete m_pDataReceiver;

	if(m_pIOVecsForPartialData)
		delete m_pIOVecsForPartialData;
}

CLStatus CLMessageReceiver::GetMessage(std::queue<CLMessage*>& qMsgContainer)
{
	void *pContext = 0;
	CLStatus s1 = m_pDataReceiver->GetData(m_pIOVecsForPartialData, &pContext);
	if(!s1.IsSuccess())
	{
		if(s1.m_lErrorCode == RECEIVED_ZERO_BYTE)
			return CLStatus(-1, MSG_RECEIVED_ZERO);
		else
			return CLStatus(-1, MSG_RECEIVED_ERROR);
	}

	if(m_pProtocolDecapsulator)
	{
		SLMessageScopeInIOVectors PartialMsgScope;
		PartialMsgScope.Index = 0;
		PartialMsgScope.Length = 0;

		vector<CLIOVectors *> vSerializedMsgs;
		CLStatus s2 = m_pProtocolDecapsulator->Decapsulate(m_pIOVecsForPartialData, vSerializedMsgs, &PartialMsgScope, pContext);
		if(!s2.IsSuccess())
		{
			for(int i = 0; i < vSerializedMsgs.size(); i++)
				delete vSerializedMsgs[i];

			return CLStatus(-1, MSG_RECEIVED_ERROR);
		}

		for(int i = 0; i < vSerializedMsgs.size(); i++)
		{
			CLMessage *pMsg = 0;
			CLStatus s3 = m_pMsgDeserializer->Deserialize(vSerializedMsgs[i], &pMsg);
			if(!s3.IsSuccess())
				return s3;

			delete vSerializedMsgs[i];
			
			qMsgContainer.push(pMsg);
		}

		if(PartialMsgScope.Length > 0)
		{
			unsigned char *ptmp = new char [PartialMsgScope.Length];
			for(int i = 0; i < PartialMsgScope.Length; i++)
				ptmp[i] = m_pIOVecsForPartialData[PartialMsgScope.Index + i];

			delete m_pIOVecsForPartialData;

			m_pIOVecsForPartialData = new CLIOVectors(true);
			m_pIOVecsForPartialData->PushBack(ptmp, PartialMsgScope.Length, true);
		}
		else
		{
			delete m_pIOVecsForPartialData;
			m_pIOVecsForPartialData = new CLIOVectors(true);
		}

		return CLStatus(0, 0);
	}

	CLMessage *pMsg1 = 0;
	CLStatus s4 = m_pMsgDeserializer->Deserialize(m_pIOVecsForPartialData, &pMsg1);
	if(!s4.IsSuccess())
		return s4;

	qMsgContainer.push(pMsg);

	delete m_pIOVecsForPartialData;
	m_pIOVecsForPartialData = new CLIOVectors(true);

	return CLStatus(0, 0);
}

CLStatus CLMessageReceiver::Decapsulate(std::vector<CLIOVectors *>& vSerializedMsgs, void *pContext)
{
	try
	{
		SLMessageScopeInIOVectors PartialMsgScope;
		PartialMsgScope.Index = 0;
		PartialMsgScope.Length = 0;

		CLStatus s1 = m_pProtocolDecapsulator->Decapsulate(m_pIOVecsForPartialData, vSerializedMsgs, &PartialMsgScope, pContext);
		if(!s1.IsSuccess())
			throw CLStatus(-1, MSG_RECEIVED_ERROR);

		//................................

		if(PartialMsgScope.Length > 0)
		{
			unsigned char *ptmp = new char [PartialMsgScope.Length];
			for(int i = 0; i < PartialMsgScope.Length; i++)
				ptmp[i] = m_pIOVecsForPartialData[PartialMsgScope.Index + i];

			delete m_pIOVecsForPartialData;

			m_pIOVecsForPartialData = new CLIOVectors(true);
			m_pIOVecsForPartialData->PushBack(ptmp, PartialMsgScope.Length, true);
		}
		else
		{
			delete m_pIOVecsForPartialData;
			m_pIOVecsForPartialData = new CLIOVectors(true);
		}

		return CLStatus(0, 0);
	}
	catch(CLStatus& s)
	{
		for(int i = 0; i < vSerializedMsgs.size(); i++)
			delete vSerializedMsgs[i];
	}
}