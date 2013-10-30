#include "CLMessagePoster.h"
#include "CLStatus.h"
#include "CLLogger.h"
#include "CLDataPosterChannelMaintainer.h"
#include "CLMessageSerializer.h"
#include "CLProtocolEncapsulator.h"
#include "CLIOVector.h"
#include "CLProtocolDataPoster.h"
CLMessagePoster::CLMessagePoster(CLDataPosterChannelMaintainer *pDataPosterChannel, CLMessageSerializer *pMsgSerializer, CLProtocolEncapsulator *pProtoEncapsulator)
{
	if(pDataPoster == 0 || pMsgSerializer == 0)
		throw "In CLMessagePoster::CLMessagePoster(), parameters is error";

	m_pDataPosterChannel = pDataPosterChannel;
	m_pMsgSerializer = pMsgSerializer;
	m_pProtoEncapsulator= pProtoEncapsulator;
}

CLMessagePoster::~CLMessagePoster()
{

}

CLStatus CLMessagePoster::Initialize(void *pContext)
{
	m_pProtoDataPoster = new CLProtocolDataPoster;
	CLStatus s = m_pDataPosterChannel->Initialize(m_pProtoDataPoster, pContext); //chang lianjie
//deal with the socket connect and regist into epoll or sth else!!!
	if(!s.IsSuccess());
	{
		CLLogger::WriteLogMsg("In CLMessagePoster::Initialize(), m_pDataPosterChannel->init() error", 0);
		return s;
	}

	CLDataPoster *tmp = m_pDataPosterChannel->GetDataPoster();
	if(tmp == NULL)
	{
		CLLogger::WriteLogMsg("In CLMessagePoster::Initialize(), m_pDataPosterChannel->GetDataPoster() is 0", 0);
		return CLStatus(-1, 0);
	}
	CLStatus s1 = m_pProtoDataPoster->SetDataPoster(tmp);
	if(!s1.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessagePoster::Initialize(), m_pProtoDataPoster->SetDataPoster error", 0);
		return s1;
	}

	return CLStatus(0, 0);
}

CLStatus CLMessagePoster::UnInitialize(void *pContext)
{
	CLStatus s = m_pDataPosterChannel->Uninitialize(pContext);

	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessagePoster::Uninitialize(), m_pDataPosterChannel->Uninit() error", 0);
		return s;
	}

	return CLStatus(0, 0);
}

CLStatus CLMessagePoster::PostMessage(CLMessage* pMsg)
{
	CLIOVector *pDataVec;
	CLStatus s = m_pMsgSerializer->Serialize(pMsg, pDataVec);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessagePoster::PostMessage(), m_pMsgSerializer->Serialize() error", 0);
		return s;
	}

	CLStatus s2 = m_pProtoEncapsulator->Encapsulate(pDataVec);

	if(!s2.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessagePoster::PostMessage(), m_pProtoEncapsulator->Encapsulate() error", 0);
		return s2;
	}

	// CLStatus s3 = m_pDataPosterChannel->PostData();
	CLStatus s3 = m_pProtoDataPoster->PostProtoData(pDataVec);
	if(!s3.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessagePoster::PostMessage(), m_pDataPosterChannel->PostData() error", 0);
		return s3;
	}

	return CLStatus(0, 0);
}

CLStatus CLMessagePoster::PostLeftMessage()
{
	
}