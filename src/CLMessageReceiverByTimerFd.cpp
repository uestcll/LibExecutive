#include "CLMessageReceiverByTimerFd.h"
#include "CLLogger.h"
#include "CLTimerOutMsg.h"
#include "CLTimerFd.h"

CLMessageReceiverByTimerFd::CLMessageReceiverByTimerFd(CLTimerFd *pTimer)
{
	try//12.2 优化：觉得这里可以封装一层CLTimerFd的类//已完成
	{
		if(!pTimer)
			throw "In CLMessageReceiverByTimerFd::CLMessageReceiverByTimerFd(), pTimer is NULL";

		m_pTimer = pTimer;

		CLStatus s = m_pTimer->StartClock();
		if(!s.IsSuccess())
			throw "In CLMessageReceiverByTimerFd::CLMessageReceiverByTimerFd(), m_pTimer->StartClock() error";
	}
	catch(...)
	{
		CLLogger::WriteLogMsg("In CLMessageReceiverByTimerFd::CLMessageReceiverByTimerFd(), ERROR", 0);
	}
}

CLMessageReceiverByTimerFd::~CLMessageReceiverByTimerFd()
{

}
 
CLStatus CLMessageReceiverByTimerFd::GetMessage(std::queue<CLMessage*> &MessageQueue)
{
	CLStatus s = m_pTimer->Read();
	if(!s.IsSuccess())
		return CLStatus(-1, 0);

	CLTimerOutMsg *pMsg = new CLTimerOutMsg();

	pMsg->m_strRemoteName = m_pTimer->GetRemoteName();
	pMsg->m_iEchoID = m_pTimer->GetTimerID();
	pMsg->m_bRepeat = m_pTimer->IsRepeatTimer();
	pMsg->m_iTimerFd = m_pTimer->GetTimerFd();

	MessageQueue.push(pMsg);

	return CLStatus(0, 0);
}

const int& CLMessageReceiverByTimerFd::GetFd()
{
	return m_pTimer->GetTimerFd();
}


