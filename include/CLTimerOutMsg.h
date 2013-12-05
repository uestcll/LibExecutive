#ifndef CLTIMEROUTMSG_H
#define CLTIMEROUTMSG_H

#include "CLMessage.h"
#include <string>
#include "MessageID.h"

class CLTimerOutMsg : public CLMessage
{
public:
	CLTimerOutMsg() : CLMessage(TIMER_OUT_MSG_ID)
	{

	}

public:
	std::string m_strRemoteName;
	int m_iEchoID; 
	bool m_bRepeat;
	int m_iTimeFd;
};

#endif