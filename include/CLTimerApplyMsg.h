#ifndef CLTIMERAPPLYMSG_H
#define CLTIMERAPPLYMSG_H

#define TIMER_APPLY_MSG_ID 40

#include <sys/timerfd.h>
#include <string>
#include "CLMessage.h"

class CLTimerApplyMsg : public CLMessage
{
public:
	CLTimerApplyMsg()	 : CLMessage(TIMER_APPLY_MSG_ID)
	{

	}

public:
	struct itimerspec m_sTimeValue;// user abslute time
	std::string m_strRemoteName;
	int m_iEchoID;
};

#endif