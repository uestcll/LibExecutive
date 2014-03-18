#ifndef CLMsgLoopManagerForShareNamedPipe_H
#define CLMsgLoopManagerForShareNamedPipe_H

#include <string>
#include "CLUuid.h"
#include "CLMessageLoopManager.h"

class CLMessageReceiver;
class CLEvent;
class CLBufferManager;
class CLProtocolDecapsulator;
class CLProtocolEncapsulator;
class CLMessageDeserializer;
class CLMessageSerializer;

class CLMsgLoopManagerForShareNamedPipe : public  CLMessageLoopManager
{
public:
	CLMsgLoopManagerForShareNamedPipe(CLMessageObserver *pMsgObserver, const char* pstrThreadName, CLProtocolEncapsulator *pEncapsulator, CLProtocolDecapsulator *pDecapsulator, CLMessageSerializer *pMsgSerializer, CLMessageDeserializer *pMsgDeserializer);
	virtual ~CLMsgLoopManagerForShareNamedPipe();

protected:
	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();

	virtual CLStatus WaitForMessage();

private:
	CLMsgLoopManagerForShareNamedPipe(const CLMsgLoopManagerForShareNamedPipe&);
	CLMsgLoopManagerForShareNamedPipe& operator=(const CLMsgLoopManagerForShareNamedPipe&);

private:
	std::string m_strThreadName;
	CLMessageReceiver *m_pMsgReceiver;
	CLUuid m_Uuid;
	CLEvent *m_pEvent;

	CLMessageSerializer *m_pMsgSerializer;
	CLMessageDeserializer *m_pMsgDeserializer;

	CLProtocolEncapsulator *m_pEncapsulator;
	CLProtocolDecapsulator *m_pDecapsulator;
};

#endif