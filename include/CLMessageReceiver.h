#ifndef CLMESSAGERECEIVER_H
#define CLMESSAGERECEIVER_H

#include <queue>
#include <CLMutex.h>

typedef struct _serialized_msg
{
	unsigned long m_lMsgID;
	int m_iStartIndex;
	int m_iLength;
	
}SLSerializedMsgScope;


class CLMessage;
class CLProtoParser;
class CLDataReceiver;
class CLBuffer;
class CLMsgDeserializerManager;

class CLMessageReceiver
{
public:
	CLMessageReceiver(CLDataReceiver *pDataReceiver, CLProtoParser *pProtoParser, CLMsgDeserializerManager *pMsgDeserializerManager);
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
	CLMsgDeserializerManager *m_pMsgDeserializerManager;

	std::queue<CLMessage*> m_MessageQueue;
	CLBuffer *m_pDataBuffer;
};

#endif

