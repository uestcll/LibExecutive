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
class CLBuffer;

class CLMessageReceiver
{
public:
	CLMessageReceiver(CLProtoParser *pProtoParser, CLDataReceiver *pDataReceiver);
	virtual ~CLMessageReceiver();

	CLMessage *GetMessage();
private:
	CLMessageReceiver(const CLMessageReceiver&);
	CLMessageReceiver& operator=(const CLMessageReceiver&);

	CLMessage* PopMessage();

protected:
	
	CLMutex m_Mutex;
	CLProtoParser *m_pProtoParser;
	CLDataReceiver *m_pDataReceiver;

	std::queue<CLMessage*> m_MessageQueue;
	CLBuffer *m_pDataBuffer;
};

#endif

