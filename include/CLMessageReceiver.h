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

class CLMessageReceiver
{
public:
	CLMessageReceiver(CLBufferManager *pBufferManager, CLDataReceiver *pDataReceiver, CLMessageDeserializer *pMsgDeserializer, CLProtocolDecapsulator *pProtocolDecapsulator = 0);
	virtual ~CLMessageReceiver();

	CLStatus GetMessage(std::queue<CLMessage*>& qMsgContainer);

private:
	CLMessageReceiver(const CLMessageReceiver&);
	CLMessageReceiver& operator=(const CLMessageReceiver&);

private:
	CLMessageDeserializer *m_pMsgDeserializer;
	CLDataReceiver *m_pDataReceiver;
	CLProtocolDecapsulator *m_pProtocolDecapsulator;
	CLBufferManager *m_pBufferManager;
};

#endif