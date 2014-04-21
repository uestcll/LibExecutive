#ifndef CLMSGLOOPMANAGERFORSOCKET_H
#define CLMSGLOOPMANAGERFORSOCKET_H

#include "CLMessageLoopManager.h"

class CLMessageDeserializer;

class CLMsgLoopManagerForSocket : public CLMessageLoopManager
{
public:
	CLMsgLoopManagerForSocket(CLMessageObserver *pMsgObserver, const char* pstrThreadName, int PipeQueueType, CLMessageDeserializer *pMsgDeserializer = NULL);
	virtual ~CLMsgLoopManagerForSocket();

	CLStatus RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer);

protected:
	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();

	virtual CLStatus WaitForMessage();

private:
	CLMsgLoopManagerForSocket(const CLMsgLoopManagerForSocket&);
	CLMsgLoopManagerForSocket& operator=(const CLMsgLoopManagerForSocket&);

private:
	CLMessageReceiver *m_pMsgReceiver;

	CLMessageDeserializer *m_pMsgDeserializer;	
};

#endif