#ifndef CLMESSAGEPOSTER_H
#define CLMESSAGEPOSTER_H

#include "CLStatus.h"

class CLDataPosterChannelMaintainer;
class CLMessageSerializer;
class CLProtocolEncapsulator;
class CLMessage;
class CLEvent;
class CLProtocolDataPoster;

class CLMessagePoster
{
public:
	CLMessagePoster(CLDataPosterChannelMaintainer *pDataPosterChannel, CLMessageSerializer *pMsgSerializer, CLProtocolEncapsulator *pProtoEncapsulator, CLEvent *pEvent);
	virtual ~CLMessagePoster();

	CLStatus Initialize(void *pContext);
	CLStatus PostMessage(CLMessage *pMsg);
	CLStatus PostLeftMessage();
	CLStatus UnInitialize(void *pContext);

private:
	CLDataPosterChannelMaintainer *m_pDataPosterChannel;
	CLMessageSerializer *m_pMsgSerializer;
	CLProtocolEncapsulator *m_pProtoEncapsulator;
	CLProtocolDataPoster *m_pProtoDataPoster;
	CLEvent	*m_pEvent;
};


#endif