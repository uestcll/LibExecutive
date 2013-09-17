#ifndef CLMessageReceiver_H
#define CLMessageReceiver_H

#include <queue>
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
	CLStatus DeserializeMsg(CLIOVectors *pIOVecs, unsigned int Index, unsigned int Length, std::queue<CLMessage*>& qMsgContainer);

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