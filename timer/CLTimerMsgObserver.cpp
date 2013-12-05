#include "LibExecutive.h"
#include <iostream>

using namespace std;

#define TIMER_PIPE_NAME "timer_server_pipe"

CLTimerMsgObserver::CLTimerMsgObserver()
{

}

CLTimerMsgObserver::~CLTimerMsgObserver()
{

}

CLStatus CLTimerMsgObserver::Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
{
	pMessageLoop->Register(TIMER_APPLY_MSG_ID, (CallBackForMessageLoop)(&CLTimerMsgObserver::On_TimerApply));
	pMessageLoop->Register(TIMER_OUT_MSG_ID, (CallBackForMessageLoop)(&CLTimerMsgObserver::On_TimerOut));

	m_pMsgLoopManager = pMessageLoop;

	CLMessagePoster sender(new CLDataPosterChannelByNamedPipeMaintainer(TIMER_PIPE_NAME, true), new CLMultiMsgSerializer(), new CLProtoEncapForDefaultMsgFormat(), new CLEvent(TIMER_PIPE_NAME, true));
	
	sender.Initialize(0);
	send.RegisterSerializer(TIMER_APPLY_MSG_ID, new CLTimerApplyMsgSerializer());

	CLTimerApplyMsg *pM = new CLTimerApplyMsg();
	memset(&(pM->m_sTimeValue), 0, sizeof(struct itimerspec));
	pM->m_sTimeValue.it_value.tv_sec = 10 + time(NULL);
	pM->m_iEchoID = 3;
	pM->m_strRemoteName = "Hello";

	send.PostMessage(pM, new CLDataPostResultNotifier(true));

	sender.UnInitialize(0);

	cout<<"Now is in CLTimerMsgObserver::Initialize(), the time is "<<time(NULL)<<endl;
	return CLStatus(0, 0);
}

CLStatus CLTimerMsgObserver::On_TimerApply(CLMessage *pM)
{
	CLTimerApplyMsg *pMsg = dynamic_cast<CLTimerApplyMsg*>(pM);
	CLStatus s = m_pMsgLoopManager->RegisterTimerReceiver(pMsg->m_sTimeValue, pMsg->m_strRemoteName, pMsg->m_iEchoID);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLTimerMsgObserver::On_TimerApply(), m_pMsgLoopManager->RegisterTimerReceiver() error", 0);
		return CLStatus(-1, 0);
	}

	return CLStatus(0, 0);
}

CLStatus CLTimerMsgObserver::On_TimerOut(CLMessage *pM)
{
	CLTimerOutMsg *pMsg = dynamic_cast<CLTimerOutMsg*>(pM); //不方便用名字来标识timerfd ，因为一个remotename可能有多个timer

	//此处先默认remote是进程之间的共有管道
	//以后再加入网络 remote ，所以timerApplyMsg里面那时候也应该加入remoteType字段。。！！！
	
	// CLMessagePoster sender(new CLDataPosterChannelByNamedPipeMaintainer(pMsg->m_strRemoteName, true), new CLMultiMsgSerializer(), new CLProtoEncapForDefaultMsgFormat(), new CLEvent(pMsg->m_strRemoteName, true));
	
	// sender.Initialize(0);
	// send.RegisterSerializer(TIMER_OUT_NOTIFY_MSG_ID, new CLTimerOutNotifyMsgSerializer());
	// send.PostMessage(new CLTimerOutNotifyMsg(pMsg->m_iEchoID), new CLDataPostResultNotifier(true));
	// sender.UnInitialize(0);
	cout<<"Now is in CLTimerMsgObserver::On_TimerOut, the time is "<<time(NULL)<<endl;
	

	if(!(pMsg->bRepeat))
	{
		CLStatus s = m_pMsgLoopManager->UnRegisterMsgReceiver(pMsg->m_TimerFd);
		if(!s.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLTimerMsgObserver::On_TimerOut(), m_pMsgLoopManager->UnRegisterMsgReceiver() error", 0);
			return CLStatus(-1, 0);
		}
	}	
	return CLStatus(0, 0);
}