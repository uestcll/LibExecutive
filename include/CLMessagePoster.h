#ifndef CLMessagePoster_H
#define CLMessagePoster_H

#include <map>
#include <uuid/uuid.h>
#include "CLStatus.h"

class CLMessageSerializer;
class CLProtocolEncapsulator;
class CLDataPoster;
class CLEvent;
class CLDataPostResultNotifier;
class CLInitialDataPostChannelNotifier;
class CLDataPostChannelMaintainer;
class CLMessage;
class CLProtocolDataPoster;

class CLMessagePoster
{
public:
	CLMessagePoster(CLMessageSerializer *pMsgSerializer, CLProtocolEncapsulator *pProtocolEncapsulator, CLDataPostChannelMaintainer *pChannelMaintainer, CLEvent *pEvent);
	virtual ~CLMessagePoster();

	CLStatus Initialize(CLInitialDataPostChannelNotifier *pNotifier, void *pContext);
	CLStatus Uninitialize(void *pContext);

	CLStatus PostMessage(CLMessage *pMsg, CLDataPostResultNotifier *pResultNotifier, CLProtocolDataPoster *pDataPoster);
	CLStatus PostLeftMessage(CLProtocolDataPoster *pDataPoster);
	CLStatus NotifyMsgPosterReadyToPost();

private:
	CLMessagePoster(const CLMessagePoster&);
	CLMessagePoster& operator=(const CLMessagePoster&);

private:
	CLMessageSerializer *m_pSerializer;
	CLProtocolEncapsulator *m_pProtocolEncapsulator;
	CLDataPostChannelMaintainer *m_pChannelMaintainer;
	CLEvent *m_pEvent;

	uuid_t m_UuidOfPoster;

	bool m_bInitial;
};

#endif