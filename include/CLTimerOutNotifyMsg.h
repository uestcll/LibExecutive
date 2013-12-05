#ifndef CLTIMEROUTNOTIFYMSG_H
#define CLTIMEROUTNOTIFYMSG_H

#include "CLMessage.h"
#include "MessageID.h"

class CLTimerOutNotifyMsg : public CLMessage
{
public:
	CLTimerOutNotifyMsg() : CLMessage(TIMER_OUT_NOTIFY_MSG_ID)
	{

	}

	explicit CLTimerOutNotifyMsg(const int& echoID) : CLMessage(TIMER_OUT_NOTIFY_MSG_ID)
	{
		m_iEchoID = echoID;
	}

public:
	int m_iEchoID; 
};

#endif