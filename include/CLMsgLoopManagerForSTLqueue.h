#ifndef CLMsgLoopManagerForSTLqueue_H
#define CLMsgLoopManagerForSTLqueue_H

#include <string>
#include "CLMessageLoopManager.h"

class CLMessageQueueBySTLqueue;

class CLMsgLoopManagerForSTLqueue : public CLMessageLoopManager
{
public:
	CLMsgLoopManagerForSTLqueue(CLMessageObserver *pMsgObserver, const char* pstrThreadName);
	virtual ~CLMsgLoopManagerForSTLqueue();

protected:
	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();
	
	virtual CLMessage* WaitForMessage();

private:
	CLMsgLoopManagerForSTLqueue(const CLMsgLoopManagerForSTLqueue&);
	CLMsgLoopManagerForSTLqueue& operator=(const CLMsgLoopManagerForSTLqueue&);

private:
	CLMessageQueueBySTLqueue*m_pMsgQueue;
	std::string m_strThreadName;
};

#endif