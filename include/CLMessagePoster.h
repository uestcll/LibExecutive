#ifndef CLMESSAGEPOSTER_H
#define CLMESSAGEPOSTER_H

#incldue "CLStatus.h"

class CLDataPosterChannelMaintainer;
class CLMessageSerializer;
class CLProtocolEncapsulator;
class CLMessage;


class CLMessagePoster
{
public:
	CLMessagePoster(CLDataPosterChannelMaintainer *pDataPosterChannel, CLMessageSerializer *pMsgSerializer, CLProtocolEncapsulator *pProtoEncapsulator);
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
};


#endif