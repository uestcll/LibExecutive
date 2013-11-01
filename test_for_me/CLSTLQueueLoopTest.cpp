#include <iostream>
#include "LibExecutive.h"

static bool g1 = false;
static bool g2 = false;
static bool g3 = false;

class CLObserverTesterForCLMsgLoopManagerForSTLqueue : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLObserverTesterForCLMsgLoopManagerForSTLqueue::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLObserverTesterForCLMsgLoopManagerForSTLqueue::On_2));
		pMessageLoop->Register(3, (CallBackForMessageLoop)(&CLObserverTesterForCLMsgLoopManagerForSTLqueue::On_3));

		CLMessagePoster *pq = (CLExecutiveNameServer::GetInstance())->GetCommunicationPtr("CLMsgLoopManagerForMsgQueueTester");

		pq->PostMessage(new CLMessage(1));
		pq->PostMessage(new CLMessage(2));
		pq->PostMessage(new CLMessage(3));

		(CLExecutiveNameServer::GetInstance())->ReleaseCommunicationPtr("CLMsgLoopManagerForMsgQueueTester");

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		std::cout<<"the msg ID is (1)"<<(pm->m_clMsgID)<<endl;
		g1 = true;
		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		std::cout<<"the msg ID is (2)"<<(pm->m_clMsgID)<<endl;
		g2 = true;
		return CLStatus(0, 0);
	}

	CLStatus On_3(CLMessage *pm)
	{
		std::cout<<"the msg ID is (2)"<<(pm->m_clMsgID)<<endl;
		g3 = true;
		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};


int main()
{

    if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
   	{
        cout<<"Initialize error"<<endl;
        return 0;
   	}

	CLMessageLoopManager *pM = new CLMsgLoopManagerForSTLqueue(new CLObserverTesterForCLMsgLoopManagerForSTLqueue, "CLMsgLoopManagerForMsgQueueTester");

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	pM->EnterMessageLoop(&s);
	
	delete pM;

	std::cout<<g1<<endl;
	std::cout<<g2<<endl;
	std::cout<<g3<<endl;

	if(!CLLibExecutiveInitializer::Destroy().IsSuccess())
   	{
            cout<<"Destroy error"<<endl;
            return -1;
   	}

	return 0;
}

