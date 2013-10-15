#ifndef CLMESSAGERECEIVER_H
#define CLMESSAGERECEIVER_H

#include <queue>
#include <CLMutex.h>

// typedef struct _serialized_msg
// {
// 	unsigned long m_lMsgID;
// 	int m_iStartIndex;
// 	int m_iLength;
	
// }SLSerializedMsgScope;


class CLMessage;
class CLProtoParser;
class CLDataReceiver;
class CLBuffer;
class CLMessageDeserializer;

class CLMessageReceiver
{
public:
	CLMessageReceiver(CLDataReceiver *pDataReceiver, CLProtoParser *pProtoParser, CLMessageDeserializer *pMsgDeserializer);
	virtual ~CLMessageReceiver();

	CLStatus GetMessage(std::queue<CLMessage*> MessageQueue);
private:
	CLMessageReceiver(const CLMessageReceiver&);
	CLMessageReceiver& operator=(const CLMessageReceiver&);

	// CLMessage* PopMessage();

protected:
	
	// CLMutex m_Mutex;
	CLProtoParser *m_pProtoParser;
	CLDataReceiver *m_pDataReceiver;
	CLMessageDeserializer *m_pMsgDeserializer;

	// std::queue<CLMessage*> m_MessageQueue;//!!!!!!put into msgloop manager!!!
	CLBuffer *m_pDataBuffer;
};

#endif

