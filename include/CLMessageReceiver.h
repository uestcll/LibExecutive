#ifndef CLMESSAGERECEIVER_H
#define CLMESSAGERECEIVER_H

#include <queue>
#include <CLMutex.h>

typedef struct _serialized_msg
{
	unsigned long m_lMsgID;
	CLIOVector m_MsgIOVec;
}SLSerializedMsg;

class CLIOVector;
class CLMessage;
class CLProtoParser;
class CLDataReceiver;

class CLMessageReceiver
{
public:
	CLMessageReceiver(CLProtoParser *pProtoParser, CLDataReceiver *pDataReceiver);
	virtual ~CLMessageReceiver();

	CLMessage *GetMessage();

	virtual CLMessage* GetMessageFromChannel() = 0;

private:
	CLMessageReceiver(const CLMessageReceiver&);
	CLMessageReceiver& operator=(const CLMessageReceiver&);

protected:
	
	CLMutex m_Mutex;
	CLProtoParser *m_pProtoParser;
	CLDataReceiver *m_pDataReceiver;

	std::queue<CLMessage*> m_MessageQueue;
	CLIOVector *m_pIOBufVec;
};

#endif

