#ifndef CLMESSAGEPOSTER_H
#define CLMESSAGEPOSTER_H

#include "CLStatus.h"

class CLDataPosterChannelMaintainer;
class CLMessageSerializer;
class CLProtocolEncapsulator;
class CLMessage;
class CLEvent;
class CLProtocolDataPoster;
class CLDataPostResultNotifier;
class CLIOVector;


class CLMessagePoster
{
public:
	CLMessagePoster(CLDataPosterChannelMaintainer *pDataPosterChannel, CLMessageSerializer *pMsgSerializer, CLProtocolEncapsulator *pProtoEncapsulator, CLEvent *pEvent);
	virtual ~CLMessagePoster();

	CLStatus Initialize(void *pContext);
	CLStatus PostMessage(CLMessage *pMsg, CLDataPostResultNotifier *pResNotifier);
	CLStatus PostLeftMessage();
	CLStatus UnInitialize(void *pContext);
	CLStatus RegisterSerializer(unsigned long lMsgID, CLMessageSerializer *pSerializer);

private:
	CLDataPosterChannelMaintainer *m_pDataPosterChannel;
	CLMessageSerializer *m_pMsgSerializer;
	CLProtocolEncapsulator *m_pProtoEncapsulator;
	CLProtocolDataPoster *m_pProtoDataPoster;
	CLEvent	*m_pEvent;
};


#endif