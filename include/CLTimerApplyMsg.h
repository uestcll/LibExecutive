#ifndef CLTIMERAPPLYMSG_H
#define CLTIMERAPPLYMSG_H

#include <sys/timerfd.h>
#include <string>
#include "CLMessage.h"
#include "MessageID.h"

class CLTimerApplyMsg : public CLMessage
{
public:
	CLTimerApplyMsg()	 : CLMessage(TIMER_APPLY_MSG_ID)
	{

	}

public:
	struct itimerspec m_sTimeValue;// user abslute time
	int m_iEchoID;
	std::string m_strRemoteName;
};

#endif