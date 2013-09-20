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

//add initialize and Uninitialize Unregister
class CLMessageReceiver
{
public:
	CLMessageReceiver(CLDataReceiver *pDataReceiver, CLMessageDeserializer *pMsgDeserializer, CLProtocolDecapsulator *pProtocolDecapsulator = 0);
	virtual ~CLMessageReceiver();

	CLStatus GetMessage(std::queue<CLMessage*>& qMsgContainer);

private:
	CLStatus Decapsulate(std::vector<CLIOVectors *>& vSerializedMsgs, void *pContext);

private:
	CLMessageReceiver(const CLMessageReceiver&);
	CLMessageReceiver& operator=(const CLMessageReceiver&);

private:
	CLMessageDeserializer *m_pMsgDeserializer;
	CLDataReceiver *m_pDataReceiver;
	CLProtocolDecapsulator *m_pProtocolDecapsulator;
	CLIOVectors *m_pIOVecsForPartialData;
};

#endif