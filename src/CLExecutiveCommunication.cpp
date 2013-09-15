#include "CLExecutiveCommunication.h"
#include "CLMessageSerializer.h"
#include "CLProtocolEncapsulator.h"
#include "CLDataPoster.h"
#include "CLEvent.h"
#include "CLLogger.h"
#include "CLProtocolDataPoster.h"
#include "CLMessage.h"

CLExecutiveCommunication::CLExecutiveCommunication(CLDataPoster *pDataPoster, CLProtocolEncapsulator *pProtocolEncapsulator, CLEvent *pEvent, CLMessageSerializer *pCommonMsgSerializer, bool bMsgDelete)
{
	try
	{
		if(pDataPoster == 0)
			throw "In CLMessagePoster::CLMessagePoster(), pDataPoster == 0 error";

		m_pCommonSerializer = pCommonMsgSerializer;
		m_pProtocolEncapsulator = pProtocolEncapsulator;
		m_pDataPoster = pDataPoster;
		m_pEvent = pEvent;
		m_bMsgDelete = bMsgDelete;

		uuid_generate(m_UuidOfPoster);

		if(m_pProtocolEncapsulator)
		{
			CLStatus s = m_pProtocolEncapsulator->Initialize(m_UuidOfPoster);
			if(!s.IsSuccess())
				throw "In CLMessagePoster::CLMessagePoster(), m_pProtocolEncapsulator->Initialize error";
		}
	}
	catch(const char *str)
	{
		if(pDataPoster)
			delete pDataPoster;

		if(pProtocolEncapsulator)
			delete pProtocolEncapsulator;

		if(pCommonMsgSerializer)
			delete pCommonMsgSerializer;

		if(pEvent)
			delete pEvent;

		throw str;
	}
}

CLExecutiveCommunication::~CLExecutiveCommunication()
{
	if(m_pDataPoster)
		delete m_pDataPoster;

	if(m_pProtocolEncapsulator)
		delete m_pProtocolEncapsulator;

	if(m_pEvent)
		delete m_pEvent;

	if(m_pCommonSerializer)
		delete m_pCommonSerializer;
	else
	{
		map<unsigned long, CLMessageSerializer*>::iterator it;
		for(it = m_SerializerTable.begin(); it != m_SerializerTable.end(); it++)
			delete it->second;
	}
}

CLStatus CLExecutiveCommunication::RegisterSerializer(unsigned long lMsgID, CLMessageSerializer *pSerializer)
{
	if(pSerializer == 0)
		return CLStatus(-1, 0);

	map<unsigned long, CLMessageSerializer*>::iterator it = m_SerializerTable.find(lMsgID);
	if(it != m_SerializerTable.end())
	{
		delete pSerializer;
		CLLogger::WriteLogMsg("In CLMessagePoster::RegisterSerializer(), m_SerializerTable.find error", 0);
		return CLStatus(-1, 0);
	}

	m_SerializerTable[lMsgID] = pSerializer;

	return CLStatus(0, 0);
}

CLStatus CLExecutiveCommunication::UnregisterSerializer(unsigned long lMsgID)
{
	map<unsigned long, CLMessageSerializer*>::iterator it = m_SerializerTable.find(lMsgID);
	if(it == m_SerializerTable.end())
		return CLStatus(-1, 0);

	delete it->second;
	m_SerializerTable.erase(it);

	return CLStatus(0, 0);
}

CLStatus CLExecutiveCommunication::Initialize(CLPostResultNotifier *pResultNotifier)
{
	CLProtocolDataPoster *pDataPoster = new CLProtocolDataPoster(m_pDataPoster, pResultNotifier, m_pEvent);

	CLStatus s = pDataPoster->Initialize();
	if(!s.IsSuccess())
		CLLogger::WriteLogMsg("In CLMessagePoster::Initialize(), pDataPoster->Initialize() error", 0);

	return s;
}

CLStatus CLExecutiveCommunication::Uninitialize()
{
	CLProtocolDataPoster *pDataPoster = new CLProtocolDataPoster(m_pDataPoster, 0, m_pEvent);

	CLStatus s1 = pDataPoster->Uninitialize();
	if(!s1.IsSuccess())
		CLLogger::WriteLogMsg("In CLMessagePoster::Uninitialize(), pDataPoster->Uninitialize() error", 0);

	if(m_pProtocolEncapsulator)
	{
		CLStatus s2 = m_pProtocolEncapsulator->Uninitialize();
		if(!s2.IsSuccess())
			CLLogger::WriteLogMsg("In CLMessagePoster::Uninitialize(), m_pProtocolEncapsulator->Uninitialize() error", 0);

		if(s1.IsSuccess() && s2.IsSuccess())
			return CLStatus(0, 0);
		else
			return CLStatus(-1, 0);
	}

	if(s1.IsSuccess())
		return CLStatus(0, 0);
	else
		return CLStatus(-1, 0);
}

CLStatus CLExecutiveCommunication::PostExecutiveMessage(CLMessage *pMsg, CLPostResultNotifier *pResultNotifier)
{
	if(pMsg == 0)
		return CLStatus(-1, 0);

	CLIOVectors *pIOV = new CLIOVectors(true);

	try
	{
		if(m_pCommonSerializer)
		{
			CLStatus s1 = m_pCommonSerializer->Serialize(pMsg, pIOV);
			if(!s1.IsSuccess())
			{
				CLLogger::WriteLogMsg("In CLMessagePoster::PostMessage(), m_pCommonSerializer->Serialize() error", 0);
				throw s1;
			}
		}
		else
		{
			map<unsigned long, CLMessageSerializer*>::iterator it = m_SerializerTable.find(pMsg->m_clMsgID);
			if(it == m_SerializerTable.end())
			{
				CLLogger::WriteLogMsg("In CLMessagePoster::PostMessage(), m_SerializerTable.find() error", 0);
				throw CLStatus(-1, 0);
			}

			CLStatus s2 = it->second->Serialize(pMsg, pIOV);
			if(!s2.IsSuccess())
			{
				CLLogger::WriteLogMsg("In CLMessagePoster::PostMessage(), it->second->Serialize error", 0);
				throw s2;
			}
		}

		if(m_pProtocolEncapsulator)
		{
			CLStatus s3 = m_pProtocolEncapsulator->Encapsulate(pIOV);
			if(!s3.IsSuccess())
			{
				CLLogger::WriteLogMsg("In CLMessagePoster::PostMessage(), m_pProtocolEncapsulator->Encapsulate error", 0);
				throw s3;
			}
		}

		CLProtocolDataPoster *pDataPoster = new CLProtocolDataPoster(m_pDataPoster, pResultNotifier, m_pEvent);

		CLStatus s4 = pDataPoster->PostProtocolData(pIOV);
		if(!s4.IsSuccess())
			CLLogger::WriteLogMsg("In CLMessagePoster::PostMessage(), pDataPoster->PostProtocolData error", 0);

		if(m_bMsgDelete)
			delete pMsg;

		return s4;
	}
	catch(CLStatus& sn)
	{
		if(pResultNotifier)
		{
			CLStatus s5 = pResultNotifier->Notify(false);
			if(!s5.IsSuccess())
				CLLogger::WriteLogMsg("In CLMessagePoster::PostMessage(), pResultNotifier->Notify error", 0);

			delete pResultNotifier;
		}

		delete pIOV;

		if(m_bMsgDelete)
			delete pMsg;

		return sn;
	}
}