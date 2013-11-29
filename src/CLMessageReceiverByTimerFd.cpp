#include "CLMessageReceiverByTimerFd.h"
#include "CLTimerApplyMsg.h"
#include "CLLogger.h"
#include "CLTimerOutMsg.h"
#include <sys/timerfd.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

CLMessageReceiverByTimerFd::CLMessageReceiverByTimerFd(CLTimerApplyMsg *pMsg)
{
	try
	{
		m_bRepeat = false;
		m_strRemoteName = pMsg->m_strRemoteName;
		m_iEchoID = pMsg->m_iEchoID;

		m_sTimerValue = pMsg->m_sTimerValue;
		if(m_sTimerValue.it_interval.tv_sec || m_sTimerValue.it_interval.tv_nsec)
			m_bRepeat = true;

		m_Fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);

		int ret = timerfd_settime(m_Fd, TFD_TIMER_ABSTIME, &m_sTimerValue, NULL);
		if(-1 == ret)
			throw "In CLMessageReceiverByTimerFd::CLMessageReceiverByTimerFd(), timerfd_settime error";
	}
	catch(...)
	{
		CLLogger::WriteLogMsg("In CLMessageReceiverByTimerFd::CLMessageReceiverByTimerFd(), ERROR");
	}
}

CLMessageReceiverByTimerFd::~CLMessageReceiverByTimerFd
{

}

CLStatus CLMessageReceiverByTimerFd::GetMessage(std::queue<CLMessage*> &MessageQueue)]
{
	CLMessage *pMsg = new CLTimerOutMsg();

	pMsg->m_strRemoteName = m_strRemoteName;
	pMsg->m_iEchoID = m_iEchoID;

	MessageQueue.push_back(pMsg);

	return CLStatus(0, 0);
}

const int& CLMessageReceiverByTimerFd::GetFd()
{
	return m_Fd;
}


