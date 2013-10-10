#ifndef CLMsgLoopManagerForSTLqueue_H
#define CLMsgLoopManagerForSTLqueue_H

#include <string>
#include "CLMessageLoopManager.h"
#include "CLEvent.h"

class CLMessageReceiverBySTLqueue;

class CLMsgLoopManagerForSTLqueue : public CLMessageLoopManager
{
public:
	/*
	pMsgObserver¾ùÓ¦´Ó¶ÑÖÐ·ÖÅä£¬ÇÒ²»±ØÏÔÊ¾µ÷ÓÃdelete
	*/
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
	CLMessageReceiver *m_pMsgReceiver;
	std::string m_strThreadName;
	CLEvent     m_Event;
};

#endif