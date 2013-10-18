#include "CLMessagePoster.h"
#include "CLStatus.h"
#include "CLLogger.h"
#include "CLDataPosterChannelMaintainer.h"
#include "CLMessageSerializer.h"
#include "CLProtocolEncapsulator.h"
#include "CLIOVector.h"

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

CLStatus CLMessagePoster::Initialize()
{
	CLStatus s = m_pDataPosterChannel->Init(); //chang lianjie
//deal with the socket connect and regist into epoll or sth else!!!
	if(!s.IsSuccess());
	{
		CLLogger::WriteLogMsg("In CLMessagePoster::Initialize(), m_pDataPosterChannel->init() error", 0);
		return s;
	}

	return CLStatus(0, 0);
}

CLStatus CLMessagePoster::Uninitialize()
{
	CLStatus s = m_pDataPosterChannel->Uninit();

	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessagePoster::Uninitialize(), m_pDataPosterChannel->Uninit() error", 0);
		return s;
	}

	return CLStatus(0, 0);
}

CLStatus CLMessagePoster::PostMessage(CLMessage* pMsg)
{
	CLIOVector dataVec;
	CLStatus s = m_pMsgSerializer->Serialize(pMsg, dataVec);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessagePoster::PostMessage(), m_pMsgSerializer->Serialize() error", 0);
		return s;
	}

	CLStatus s2 = m_pProtoEncapsulator->Encapsulate(dataVec);

	if(!s2.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessagePoster::PostMessage(), m_pProtoEncapsulator->Encapsulate() error", 0);
		return s2;
	}

	CLStatus s3 = m_pDataPosterChannel->PostData(dataVec);
	if(!s3.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessagePoster::PostMessage(), m_pDataPosterChannel->PostData() error", 0);
		return s3;
	}

	return CLStatus(0, 0);
}