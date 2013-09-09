#include <memory.h>
#include "CLMessageReceiver.h"
#include "CLMessageDeserializer.h"
#include "CLProtocolDecapsulator.h"
#include "CLDataReceiver.h"
#include "CLIOVectors.h"
#include "CLLogger.h"

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
	if(m_pDataReceiver)
		delete m_pDataReceiver;

	if(m_pProtocolDecapsulator)
		delete m_pProtocolDecapsulator;
	
	if(m_pMsgDeserializer)
		delete m_pMsgDeserializer;

	if(m_pIOVecsForPartialData)
		delete m_pIOVecsForPartialData;
}

CLStatus CLMessageReceiver::GetMessage(std::queue<CLMessage*>& qMsgContainer)
{
	void *pContext = 0;
	CLStatus s1 = m_pDataReceiver->GetData(m_pIOVecsForPartialData, &pContext);
	if(!s1.IsSuccess())
		return s1;

	if(m_pProtocolDecapsulator)
	{
		vector<SLMessageScopeInIOVectors> vMsgsScope;
		SLMessageScopeInIOVectors PartialMsgScope;
		PartialMsgScope.Index = 0;
		PartialMsgScope.Length = 0;

		CLStatus s2 = m_pProtocolDecapsulator->Decapsulate(m_pIOVecsForPartialData, vMsgsScope, &PartialMsgScope, pContext);
		if(!s2.IsSuccess())
			return s2;

		for(int i = 0; i < vMsgsScope.size(); i++)
		{
			CLStatus s3 = DeserializeMsg(m_pIOVecsForPartialData, vMsgsScope[i].Index, vMsgsScope[i].Length, qMsgContainer);
			if(!s3.IsSuccess())
				return s3;
		}

		if(PartialMsgScope.Length > 0)
		{
			unsigned char *ptmp = new char [PartialMsgScope.Length];
			for(int i = 0; i < PartialMsgScope.Length; i++)
				ptmp[i] = m_pIOVecsForPartialData[PartialMsgScope.Index + i];

			delete m_pIOVecsForPartialData;

			m_pIOVecsForPartialData = new CLIOVectors(true);
			m_pIOVecsForPartialData->PushBack(ptmp, PartialMsgScope.Length);
		}
		else
		{
			delete m_pIOVecsForPartialData;
			m_pIOVecsForPartialData = new CLIOVectors(true);
		}

		return CLStatus(0, 0);
	}

	return DeserializeMsg(&iovecs, 0, iovecs.Size(), qMsgContainer);
}

CLStatus CLMessageReceiver::DeserializeMsg(CLIOVectors *pIOVecs, unsigned int Index, unsigned int Length, std::queue<CLMessage*>& qMsgContainer)
{
	CLMessage *pMsg = 0;
	CLStatus s = m_pMsgDeserializer->Deserialize(pIOVecs, Index, Length, &pMsg);
	if(!s.IsSuccess())
		return s;

	qMsgContainer.push(pMsg);

	return CLStatus(0, 0);
}