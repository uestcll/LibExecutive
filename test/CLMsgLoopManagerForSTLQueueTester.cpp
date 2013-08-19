#include <gtest/gtest.h>
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

		CLExecutiveCommunication *pq = (CLExecutiveNameServer::GetInstance())->GetCommunicationPtr("CLMsgLoopManagerForMsgQueueTester");

		EXPECT_TRUE((pq->PostExecutiveMessage(new CLMessage(1))).IsSuccess());
		EXPECT_TRUE((pq->PostExecutiveMessage(new CLMessage(2))).IsSuccess());
		EXPECT_TRUE((pq->PostExecutiveMessage(new CLMessage(3))).IsSuccess());

		EXPECT_TRUE(((CLExecutiveNameServer::GetInstance())->ReleaseCommunicationPtr("CLMsgLoopManagerForMsgQueueTester")).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		EXPECT_EQ(pm->m_clMsgID, 1);
		g1 = true;
		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		EXPECT_EQ(pm->m_clMsgID, 2);
		g2 = true;
		return CLStatus(0, 0);
	}

	CLStatus On_3(CLMessage *pm)
	{
		EXPECT_EQ(pm->m_clMsgID, 3);
		g3 = true;
		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

class CLObserverTester1ForCLMsgLoopManagerForSTLqueue : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		CLLogger::WriteLogMsg("MessageObserver Initialize error is a test", 0);
		return CLStatus(-1, 0);
	}
};

TEST(CLMsgLoopManagerForSTLqueue, Normal)
{
	CLMessageLoopManager *pM = new CLMsgLoopManagerForSTLqueue(new CLObserverTesterForCLMsgLoopManagerForSTLqueue, "CLMsgLoopManagerForMsgQueueTester");

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	pM->EnterMessageLoop(&s);
	
	delete pM;

	EXPECT_TRUE(g1);
	EXPECT_TRUE(g2);
	EXPECT_TRUE(g3);
}

TEST(CLMsgLoopManagerForSTLqueue, ObserverInitializeFailure)
{
	CLMessageLoopManager *pM = new CLMsgLoopManagerForSTLqueue(new CLObserverTester1ForCLMsgLoopManagerForSTLqueue, "CLMsgLoopManagerForMsgQueueTester");

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	EXPECT_FALSE((pM->EnterMessageLoop(&s)).IsSuccess());
	
	delete pM;
}