#ifndef CLMsgLoopManagerForPrivateNamedPipe_H
#define CLMsgLoopManagerForPrivateNamedPipe_H

#include <string>
#include "CLUuid.h"
#include "CLMessageLoopManager.h"

class CLMessageReceiver;
class CLEvent;
class CLBufferManager;

class CLMsgLoopManagerForPrivateNamedPipe : public CLMessageLoopManager
{
public:
	CLMsgLoopManagerForPrivateNamedPipe(CLMessageObserver *pMsgObserver, const char* pstrThreadName);
	virtual ~CLMsgLoopManagerForPrivateNamedPipe();

protected:
	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();

	virtual CLStatus WaitForMessage();

private:
	CLMsgLoopManagerForPrivateNamedPipe(const CLMsgLoopManagerForPrivateNamedPipe&);
	CLMsgLoopManagerForPrivateNamedPipe& operator=(const CLMsgLoopManagerForPrivateNamedPipe&);

private:
	std::string m_strThreadName;
	CLMessageReceiver *m_pMsgReceiver;
	CLUuid m_Uuid;
	CLEvent *m_pEvent;
};

#endif