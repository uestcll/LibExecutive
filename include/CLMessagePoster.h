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

	CLStatus Initialize();
	CLStatus PostMessage(CLMessage *pMsg);
	CLStatus Uninitialize();

private:
	CLDataPosterChannelMaintainer *m_pDataPosterChannel;
	CLMessageSerializer *m_pMsgSerializer;
	CLProtocolEncapsulator *m_pProtoEncapsulator;
};


#endif