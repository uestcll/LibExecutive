#ifndef CLMsgLoopManagerForPrivateNamedPipe_H
#define CLMsgLoopManagerForPrivateNamedPipe_H

#include <string>
#include "CLMessageLoopManager.h"

class CLMsgReceiverFromPrivateNamedPipe;

class CLMsgLoopManagerForPrivateNamedPipe : public CLMessageLoopManager
{
public:
	CLMsgLoopManagerForPrivateNamedPipe(CLMessageObserver *pMsgObserver, const char* pstrThreadName);
	virtual ~CLMsgLoopManagerForPrivateNamedPipe();

protected:
	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();

	virtual CLMessage* WaitForMessage();

private:
	CLMsgLoopManagerForPrivateNamedPipe(const CLMsgLoopManagerForPrivateNamedPipe&);
	CLMsgLoopManagerForPrivateNamedPipe& operator=(const CLMsgLoopManagerForPrivateNamedPipe&);

private:
	CLMsgReceiverFromPrivateNamedPipe *m_pMsgQueue;
	std::string m_strThreadName;
};

#endif