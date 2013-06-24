#include <gtest/gtest.h>
#include <signal.h>
#include <unistd.h>
#include "LibExecutive.h"

#define TEST_MESSAGE_IDForPrivateCommByNamedPipe 1
#define QUIT_MESSAGE_IDForPrivateCommByNamedPipe 2

static int count = 0;

class CLTestMsgForPrivateCommByNamedPipe : public CLMessage
{
public:
	CLTestMsgForPrivateCommByNamedPipe() : CLMessage(TEST_MESSAGE_IDForPrivateCommByNamedPipe)
	{   
		i = 2;
		j = 3;
	}   

	virtual ~CLTestMsgForPrivateCommByNamedPipe()
	{   
		count++;
	}   

	int i;
	int j;
};

class CLQuitMsgForPrivateCommByNamedPipe : public CLMessage
{
public:
	CLQuitMsgForPrivateCommByNamedPipe() : CLMessage(QUIT_MESSAGE_IDForPrivateCommByNamedPipe)
	{
	}

	virtual ~CLQuitMsgForPrivateCommByNamedPipe()
	{
		count++;
	}
};

class CLTestObserverForPrivateCommByNamedPipe : public CLMessageObserver
{
public:
	CLTestObserverForPrivateCommByNamedPipe()
	{   
	}   

	virtual ~CLTestObserverForPrivateCommByNamedPipe()
	{   
	}   

	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{   
		pMessageLoop->Register(TEST_MESSAGE_IDForPrivateCommByNamedPipe, (CallBackForMessageLoop)(&CLTestObserverForPrivateCommByNamedPipe::On_Test));
		pMessageLoop->Register(QUIT_MESSAGE_IDForPrivateCommByNamedPipe, (CallBackForMessageLoop)(&CLTestObserverForPrivateCommByNamedPipe::On_Quit));

		return CLStatus(0, 0); 
	}   

	CLStatus On_Test(CLMessage *pm)
	{   
		CLTestMsgForPrivateCommByNamedPipe *p = dynamic_cast<CLTestMsgForPrivateCommByNamedPipe*>(pm);
		if(p == 0)
			return CLStatus(0, 0); 

		return CLStatus(0, 0);
	}

	CLStatus On_Quit(CLMessage *pm)
	{
		CLQuitMsgForPrivateCommByNamedPipe *p = dynamic_cast<CLQuitMsgForPrivateCommByNamedPipe*>(pm);
		if(p == 0)
			return CLStatus(0, 0);

		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

TEST(CLPrivateExecutiveCommunicationByNamedPipe, Normal)
{
	signal(SIGPIPE, SIG_IGN);

	CLThreadForMsgLoop thread(new CLTestObserverForPrivateCommByNamedPipe, "test_pipeForPrivateCommByNamedPipe", false, EXECUTIVE_IN_PROCESS_USE_PIPE_QUEUE);
	thread.Run(0);

	CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
	CLExecutiveCommunication *q = p->GetCommunicationPtr("test_pipeForPrivateCommByNamedPipe");

	q->PostExecutiveMessage(new CLTestMsgForPrivateCommByNamedPipe());
	q->PostExecutiveMessage(new CLTestMsgForPrivateCommByNamedPipe());
	q->PostExecutiveMessage(new CLQuitMsgForPrivateCommByNamedPipe());

	sleep(3);

	q->PostExecutiveMessage(new CLTestMsgForPrivateCommByNamedPipe());

	p->ReleaseCommunicationPtr("test_pipeForPrivateCommByNamedPipe");

	EXPECT_EQ(count, 4);
}