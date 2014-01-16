#include "CLMessagePoster.h"
#include "CLMessageSerializer.h"
#include "CLProtocolEncapsulator.h"
#include "CLDataPoster.h"
#include "CLEvent.h"
#include "CLLogger.h"
#include "CLProtocolDataPoster.h"
#include "CLMessage.h"
#include "CLDataPostChannelMaintainer.h"
#include "CLDataPostResultNotifier.h"
#include "CLInitialDataPostChannelNotifier.h"
#include "ErrorCode.h"
#include "CLIOVectors.h"

CLMessagePoster::CLMessagePoster(CLMessageSerializer *pMsgSerializer, CLProtocolEncapsulator *pProtocolEncapsulator, CLDataPostChannelMaintainer *pChannelMaintainer, CLEvent *pEvent)
{
	try
	{
		if((pMsgSerializer == 0) || (pChannelMaintainer == 0))
			throw "In CLMessagePoster::CLMessagePoster(), parameters error";

		m_pSerializer = pMsgSerializer;
		m_pProtocolEncapsulator = pProtocolEncapsulator;
		m_pChannelMaintainer = pChannelMaintainer;
		m_pEvent = pEvent;

		uuid_generate(m_UuidOfPoster);

		m_bInitial = false;
	}
	catch(const char *str)
	{
		if(pMsgSerializer)
			delete pMsgSerializer;

		if(pProtocolEncapsulator)
			delete pProtocolEncapsulator;

		if(pChannelMaintainer)
			delete pChannelMaintainer;

		if(pEvent)
			delete pEvent;

		throw str;
	}
}

CLMessagePoster::~CLMessagePoster()
{
	if(m_pSerializer)
		delete m_pSerializer;

	if(m_pProtocolEncapsulator)
		delete m_pProtocolEncapsulator;

	if(m_pChannelMaintainer)
		delete m_pChannelMaintainer;

	if(m_pEvent)
		delete m_pEvent;
}

CLStatus CLMessagePoster::Initialize(CLInitialDataPostChannelNotifier *pNotifier, void *pContext)
{
	if(pNotifier == 0)
		return CLStatus(-1, MSG_INITIALIZE_ERROR);

	if(m_bInitial)
	{
		pNotifier->Notify(MSG_INITIALIZE_SUCCESS);
		delete pNotifier;
		return CLStatus(0, 0);
	}

	if(m_pProtocolEncapsulator)
	{
		CLStatus s = m_pProtocolEncapsulator->Initialize(m_UuidOfPoster);
		if(!s.IsSuccess())
		{
			pNotifier->Notify(MSG_INITIALIZE_ERROR);
			delete pNotifier;
			CLLogger::WriteLogMsg("In CLMessagePoster::Initialize(), m_pProtocolEncapsulator->Initialize error", 0);
			return CLStatus(-1, MSG_INITIALIZE_ERROR);
		}
	}

	CLStatus s1 = m_pChannelMaintainer->Initialize(pNotifier, pContext);
	if(!s1.IsSuccess() && (s1.m_clErrorCode == DATA_POSTER_INITIALIZE_ERROR))
	{
		CLLogger::WriteLogMsg("In CLMessagePoster::Initialize(), m_pChannelMaintainer->Initialize error", 0);
		return CLStatus(-1, MSG_INITIALIZE_ERROR);
	}

	m_bInitial = true;
	return s1;
}

CLStatus CLMessagePoster::Uninitialize(void *pContext)
{
	if(!m_bInitial)
		return CLStatus(-1, NORMAL_ERROR);

	CLStatus s1 = m_pChannelMaintainer->Uninitialize(pContext);
	if(!s1.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessagePoster::Uninitialize(), m_pChannelMaintainer->Uninitialize() error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	if(m_pProtocolEncapsulator)
	{
		CLStatus s2 = m_pProtocolEncapsulator->Uninitialize();
		if(!s2.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLMessagePoster::Uninitialize(), m_pProtocolEncapsulator->Uninitialize() error", 0);
			return CLStatus(-1, NORMAL_ERROR);
		}
	}

	return CLStatus(0, 0);
}

CLStatus CLMessagePoster::PostMessage(CLMessage *pMsg, CLDataPostResultNotifier *pResultNotifier, CLProtocolDataPoster *pDataPoster)
{
	CLIOVectors *pIOV = 0;

	try
	{
		if(pResultNotifier == 0)
			throw CLStatus(-1, MSG_POST_ERROR);

		pResultNotifier->SetMsg(pMsg);

		if((pMsg == 0) || (pDataPoster == 0) || (pDataPoster->IsSetParameters()))
			throw CLStatus(-1, MSG_POST_ERROR);

		pIOV = new CLIOVectors();

		CLStatus s1 = m_pSerializer->Serialize(pMsg, pIOV);
		if(!s1.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLMessagePoster::PostMessage(), m_pSerializer->Serialize() error", 0);
			throw CLStatus(-1, MSG_POST_ERROR);
		}

		if(m_pProtocolEncapsulator)
		{
			CLStatus s2 = m_pProtocolEncapsulator->Encapsulate(pIOV);
			if(!s2.IsSuccess())
			{
				CLLogger::WriteLogMsg("In CLMessagePoster::PostMessage(), m_pProtocolEncapsulator->Encapsulate error", 0);
				throw CLStatus(-1, MSG_POST_ERROR);
			}
		}

		CLDataPoster *ptmp = m_pChannelMaintainer->GetDataPoster();
		if(ptmp == 0)
		{
			CLLogger::WriteLogMsg("In CLMessagePoster::PostMessage(), m_pChannelMaintainer->GetDataPoster error", 0);
			throw CLStatus(-1, MSG_POST_ERROR);
		}

		ptmp->SetProtocolDataPoster(pDataPoster);

		pDataPoster->SetParameters(ptmp, pResultNotifier, m_pEvent);
		CLStatus s3 = pDataPoster->PostProtocolData(pIOV);
		if(!s3.IsSuccess() && (s3.m_clErrorCode == DATA_POSTER_POST_ERROR))
			CLLogger::WriteLogMsg("In CLMessagePoster::PostMessage(), pDataPoster->PostProtocolData error", 0);

		return s3;
	}
	catch(CLStatus& s)
	{
		if(pResultNotifier == 0)
		{
			if(pMsg)
				delete pMsg;
		}
		else
		{
			pResultNotifier->Notify(MSG_POST_ERROR);
			delete pResultNotifier;
		}

		if(pDataPoster)
			delete pDataPoster;

		if(pIOV)
			delete pIOV;

		return s;
	}
}

CLStatus CLMessagePoster::PostLeftMessage(CLProtocolDataPoster *pDataPoster)
{
	if((pDataPoster == 0) || (pDataPoster->IsSetParameters() == false))
	{
		if(pDataPoster)
			delete pDataPoster;

		return CLStatus(-1, MSG_POST_ERROR);
	}

	CLStatus s = pDataPoster->PostLeftProtocolData();
	if(!s.IsSuccess() && (s.m_clErrorCode == DATA_POSTER_POST_ERROR))
		CLLogger::WriteLogMsg("In CLMessagePoster::PostLeftMessage(), pDataPoster->PostLeftProtocolData error", 0);

	return s;
}