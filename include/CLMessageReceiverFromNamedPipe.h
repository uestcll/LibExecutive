#ifndef CLMessageReceiverFromNamedPipe_H
#define CLMessageReceiverFromNamedPipe_H

#include "CLMessageReceiver.h"

class CLMessage;

using namespace std;

class CLMessageReceiverFromNamedPipe : public CLMessageReceiver
{
public:
	CLMessageReceiverFromNamedPipe(CLMessageDeserializer *pMsgDeserializer, const char *pstrPipeName);
	virtual ~CLMessageReceiverFromNamedPipe();

	virtual CLStatus GetMessage(CLMessage **ppMsg);

	CLMessage *GetMessage();

protected:
	virtual CLMessage *ReadMsgFromPipe(int fd) = 0;

private:
	CLMessageReceiverFromNamedPipe(const CLMessageReceiverFromNamedPipe&);
	CLMessageReceiverFromNamedPipe& operator=(const CLMessageReceiverFromNamedPipe&);

private:
	int m_Fd;
};

#endif