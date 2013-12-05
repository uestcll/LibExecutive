#include <iostream>
#include "LibExecutive.h"
#include "CLTimerMsgObserver.h"



using namespace std;

int main()
{
	if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
   	{
        cout<<"Initialize error"<<endl;
        return 0;
   	}
   	CLEpoll *pEpoll = new CLEpoll;
   	CLStatus s = m_pEpoll->Initialize(EPOLL_MAX_FD_SIZE);
   	if(!s.IsSuccess())
   		cout<<"epoll Initialize error"<<endl;

	CLMsgLoopManagerForEpoll *pM = new CLMsgLoopManagerForEpoll(new CLTimerMsgObserver(), pEpoll);
	pM->RegisterDeserializer(TIMER_APPLY_MSG_ID, new CLTimerApplyMsgDeserializer());

	CLStatus s1 = pM->RegisterPipeReceiver(TIMER_PIPE_NAME, PIPE_QUEUE_BETWEEN_PROCESS);
	if(!s1.IsSuccess())
		cout<<"RegisterPipeReceiver error"<<endl;

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	// EXPECT_TRUE((pM->EnterMessageLoop(&s)).IsSuccess());
	pM->EnterMessageLoop(&s);
	delete pM;

	return 0;
}