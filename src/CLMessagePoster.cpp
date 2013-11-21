#include "CLMessagePoster.h"
#include "CLStatus.h"
#include "CLLogger.h"
#include "CLDataPosterChannelMaintainer.h"
#include "CLMessageSerializer.h"
#include "CLProtocolEncapsulator.h"
#include "CLIOVector.h"
#include "CLProtocolDataPoster.h"
#include "CLEvent.h"
#include "CLDataPoster.h"
#include "CLMultiMsgSerializer.h"
#include "CLDataPostResultNotifier.h"

CLMessagePoster::CLMessagePoster(CLDataPosterChannelMaintainer *pDataPosterChannel, CLMessageSerializer *pMsgSerializer, CLProtocolEncapsulator *pProtoEncapsulator, CLEvent *pEvent)
{
	if(pDataPosterChannel == 0 || pMsgSerializer == 0)
		throw "In CLMessagePoster::CLMessagePoster(), parameters is error";

	m_pDataPosterChannel = pDataPosterChannel;
	m_pMsgSerializer = pMsgSerializer;
	m_pProtoEncapsulator= pProtoEncapsulator;
	m_pEvent = pEvent;
	m_pProtoDataPoster = new CLProtocolDataPoster;
}

CLMessagePoster::~CLMessagePoster()
{
	if(m_pEvent)
	{
		delete m_pEvent;
		m_pEvent = 0;
	}
	if(m_pDataPosterChannel)
	{
		delete m_pDataPosterChannel;
		m_pDataPosterChannel = 0;
	}
	if(m_pMsgSerializer)
	{
		delete m_pMsgSerializer;
		m_pMsgSerializer = 0;
	}
	if(m_pProtoEncapsulator)
	{
		delete m_pProtoEncapsulator;
		m_pProtoEncapsulator = 0;
	}
	if(m_pProtoDataPoster)
	{
		delete m_pProtoDataPoster;
		m_pProtoDataPoster = 0;
	}
}

CLStatus CLMessagePoster::Initialize(void *pContext)
{
	CLStatus s = m_pDataPosterChannel->Initialize(pContext); //chang lianjie
//deal with the socket connect and regist into epoll or sth else!!!
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessagePoster::Initialize(), m_pDataPosterChannel->init() error", 0);
		return s;
	}

	CLDataPoster *tmpDataPoster = m_pDataPosterChannel->GetDataPoster();
	if(tmpDataPoster == NULL)
	{
		CLLogger::WriteLogMsg("In CLMessagePoster::Initialize(), m_pDataPosterChannel->GetDataPoster() is 0", 0);
		return CLStatus(-1, 0);
	}
	CLStatus s1 = m_pProtoDataPoster->SetParameters(tmpDataPoster, m_pEvent);
	if(!s1.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessagePoster::Initialize(), m_pProtoDataPoster->SetDataPoster error", 0);
		return s1;
	}
	tmpDataPoster->SetProtocolDataPoster(m_pProtoDataPoster);

	return CLStatus(0, 0);
}

CLStatus CLMessagePoster::UnInitialize(void *pContext)
{
	CLStatus s = m_pDataPosterChannel->UnInitialize(pContext);

	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessagePoster::Uninitialize(), m_pDataPosterChannel->Uninit() error", 0);
		return s;
	}

	return CLStatus(0, 0);
}

CLStatus CLMessagePoster::PostMessage(CLMessage* pMsg, CLDataPostResultNotifier *pResNotifier)
{
	CLIOVector *pDataVec = new CLIOVector();
	CLStatus s = m_pMsgSerializer->Serialize(pMsg, pDataVec);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessagePoster::PostMessage(), m_pMsgSerializer->Serialize() error", 0);
		return s;
	}

	if(m_pProtoEncapsulator)
	{
		CLStatus s2 = m_pProtoEncapsulator->Encapsulate(pDataVec);

		if(!s2.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLMessagePoster::PostMessage(), m_pProtoEncapsulator->Encapsulate() error", 0);
			return s2;
		}
	}
	// CLStatus s3 = m_pDataPosterChannel->PostData();
	pResNotifier->SetMessage(pMsg);
	
	SLDataAndNotifier *pDataAndNotifier = new SLDataAndNotifier;
	pDataAndNotifier->m_pData = pDataVec;
	pDataAndNotifier->m_pResNotifier = pResNotifier;
	
	CLStatus s3 = m_pProtoDataPoster->PostProtoData(pDataAndNotifier);
	if(s3.IsSuccess())
	{
		return CLStatus(0, POST_MSG_SUCCESS);
	}
	else if(s3.m_clErrorCode == POST_DATA_ERROR)
	{
		CLLogger::WriteLogMsg("In CLMessagePoster::PostMessage(), m_pDataPosterChannel->PostData() error", 0);
		return CLStatus(-1, POST_MSG_ERROR);
	}
	else
	{
		return CLStatus(-1, POST_MSG_PARTION);
	}
}

CLStatus CLMessagePoster::PostLeftMessage()
{
	CLStatus s = m_pProtoDataPoster->PostLeftProtoData();
	if(!s.IsSuccess() && (s.m_clErrorCode == POST_DATA_ERROR))
		CLLogger::WriteLogMsg("In CLMessagePoster::PostLeftMessage(), m_pProtoDataPoster->PostLeftProtoData() error", 0);

	return s;
}

CLStatus CLMessagePoster::RegisterSerializer(unsigned long lMsgID, CLMessageSerializer *pSerializer)
{
	CLMultiMsgSerializer *pMultiSerializer = dynamic_cast<CLMultiMsgSerializer *>(m_pMsgSerializer);
	if(pMultiSerializer == 0)
		return CLStatus(0, 0);

	return (pMultiSerializer->RegisterSerializer(lMsgID, pSerializer));
}