#ifndef CLMsgReceiverFromPrivateNamedPipe_H
#define CLMsgReceiverFromPrivateNamedPipe_H

#include "CLMessageReceiver.h"

class CLNamedPipe;

class CLMsgReceiverFromPrivateNamedPipe : public CLMessageReceiver
{
public:
	CLMsgReceiverFromPrivateNamedPipe(CLMessageDeserializer *pMsgDeserializer, const char *pstrPipeName);
	virtual ~CLMsgReceiverFromPrivateNamedPipe();

	virtual CLStatus GetMessage(CLMessage **ppMsg);

private:
	CLMsgReceiverFromPrivateNamedPipe(const CLMsgReceiverFromPrivateNamedPipe&);
	CLMsgReceiverFromPrivateNamedPipe& operator=(const CLMsgReceiverFromPrivateNamedPipe&);

private:
	CLNamedPipe *m_pNamedPipe;
};

#endif