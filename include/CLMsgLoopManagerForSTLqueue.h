#ifndef CLMsgLoopManagerForSTLqueue_H
#define CLMsgLoopManagerForSTLqueue_H

#include <string>
#include "CLMessageLoopManager.h"

class CLMessageReceiver;
class CLNotify;
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
	CLNotify *m_pEvent;
	CLSTLqueue *m_pSTLqueue;
};

#endif
