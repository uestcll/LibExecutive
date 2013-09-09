#ifndef CLMESSAGERECEIVER_H
#define CLMESSAGERECEIVER_H

#include <queue>
#include <CLMutex.h>

class CLMessage;
class CLProtoParser;

class CLMessageReceiver
{
public:
	CLMessageReceiver(CLProtoParser *pProtoParser);
	virtual ~CLMessageReceiver();

	CLMessage *GetMessage();

	virtual CLMessage* GetMessageFromChannel() = 0;

	const int& m_ciFd;
private:
	CLMessageReceiver(const CLMessageReceiver&);
	CLMessageReceiver& operator=(const CLMessageReceiver&);

protected:
	int m_Fd;
	
	CLMutex m_Mutex;
	CLProtoParser *m_pProtoParser;
	std::queue<CLMessage*> m_MessageQueue;
};

#endif

