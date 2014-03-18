#ifndef CLMessageReceiver_H
#define CLMessageReceiver_H

#include <queue>
#include <vector>
#include "CLStatus.h"

class CLMessage;
class CLMessageDeserializer;
class CLProtocolDecapsulator;
class CLDataReceiver;
class CLIOVectors;
class CLBufferManager;
class CLUuid;
struct SLMessageAndSource;

class CLMessageReceiver
{
public:
	CLMessageReceiver(CLUuid *pChannelUuid, CLBufferManager *pBufferManager, CLDataReceiver *pDataReceiver, CLMessageDeserializer *pMsgDeserializer, CLProtocolDecapsulator *pProtocolDecapsulator = 0);
	virtual ~CLMessageReceiver();

	CLStatus GetMessage(std::queue<SLMessageAndSource*>& qMsgContainer);

private:
	CLMessageReceiver(const CLMessageReceiver&);
	CLMessageReceiver& operator=(const CLMessageReceiver&);

private:
	CLMessageDeserializer *m_pMsgDeserializer;
	CLDataReceiver *m_pDataReceiver;
	CLProtocolDecapsulator *m_pProtocolDecapsulator;
	CLBufferManager *m_pBufferManager;
	
	CLUuid *m_pChannelUuid;
};

#endif