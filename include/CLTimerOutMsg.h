#ifndef CLTIMEROUTMSG_H
#define CLTIMEROUTMSG_H

#define TIMER_OUT_MSG_ID 41

#include "CLMessage.h"
#include <string>

class CLTimerOutMsg : public CLMessage
{
public:
	CLTimerOutMsg() : CLMessage(TIMER_OUT_MSG_ID)
	{

	}
public:
	std::string m_strRemoteName;
	int m_iEchoID; 
};

#endif