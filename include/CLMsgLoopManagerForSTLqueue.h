#ifndef CLMsgLoopManagerForSTLqueue_H
#define CLMsgLoopManagerForSTLqueue_H

#include <string>
#include "CLUuid.h"
#include "CLMessageLoopManager.h"

class CLMessageReceiver;
class CLEvent;
class CLBufferManager;
class CLSTLqueue;

class CLMsgLoopManagerForSTLqueue : public CLMessageLoopManager
{
public:
	CLMsgLoopManagerForSTLqueue(CLMessageObserver *pMsgObserver, const char* pstrThreadName);
	virtual ~CLMsgLoopManagerForSTLqueue();

protected:
	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();
	
	virtual CLStatus WaitForMessage();

private:
	CLMsgLoopManagerForSTLqueue(const CLMsgLoopManagerForSTLqueue&);
	CLMsgLoopManagerForSTLqueue& operator=(const CLMsgLoopManagerForSTLqueue&);

private:
	std::string m_strThreadName;
	CLMessageReceiver *m_pMsgReceiver;
	CLEvent *m_pEvent;
	CLUuid m_Uuid;
	CLSTLqueue *m_pSTLqueue;
};

#endif