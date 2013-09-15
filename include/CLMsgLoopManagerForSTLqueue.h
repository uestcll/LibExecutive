#ifndef CLMsgLoopManagerForSTLqueue_H
#define CLMsgLoopManagerForSTLqueue_H

#include <string>
#include "CLMessageLoopManager.h"

class CLMessageReceiver;
class CLEvent;

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
	CLMessageReceiver *m_pMsgReceiver;
	std::string m_strThreadName;
	CLEvent *m_pEvent;
};

#endif