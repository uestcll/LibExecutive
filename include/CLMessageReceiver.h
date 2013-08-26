#ifndef CLMessageReceiver_H
#define CLMessageReceiver_H

#include "CLStatus.h"

#define MESSAGE_RECEIVE_ERROR 2
#define MESSAGE_RECEIVE_NOT_ENOUGH 1

class CLMessage;
class CLMessageDeserializer;

class CLMessageReceiver
{
public:
	CLMessageReceiver(CLMessageDeserializer *pMsgDeserializer);
	virtual ~CLMessageReceiver();

	virtual CLStatus GetMessage(CLMessage **ppMsg) = 0;

private:
	CLMessageReceiver(const CLMessageReceiver&);
	CLMessageReceiver& operator=(const CLMessageReceiver&);

private:
	CLMessageDeserializer *m_pMsgDeserializer;
};

#endif