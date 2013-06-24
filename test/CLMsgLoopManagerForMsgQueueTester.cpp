#include <gtest/gtest.h>
#include "CLMsgLoopManagerForSTLqueue.h"
#include "CLMessageObserver.h"
#include "CLMessageQueueBySTLqueue.h"
#include "CLMessage.h"
#include "CLExecutiveNameServer.h"
#include "CLExecutiveCommunication.h"
#include "CLThreadInitialFinishedNotifier.h"

class CLObserverTester : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLObserverTester::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLObserverTester::On_2));
		pMessageLoop->Register(3, (CallBackForMessageLoop)(&CLObserverTester::On_3));

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
		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		EXPECT_EQ(pm->m_clMsgID, 2);
		return CLStatus(0, 0);
	}

	CLStatus On_3(CLMessage *pm)
	{
		EXPECT_EQ(pm->m_clMsgID, 3);
		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

class CLObserverTester1 : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		return CLStatus(-1, 0);
	}
};

TEST(CLMsgLoopManagerForMsgQueueTester, Normal)
{
	CLMessageLoopManager *pM = new CLMsgLoopManagerForSTLqueue(new CLObserverTester, "CLMsgLoopManagerForMsgQueueTester");

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	pM->EnterMessageLoop(&s);
	
	delete pM;
}

TEST(CLMsgLoopManagerForMsgQueueTester, ObserverInitializeFailure)
{
	CLMessageLoopManager *pM = new CLMsgLoopManagerForSTLqueue(new CLObserverTester1, "CLMsgLoopManagerForMsgQueueTester");

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	EXPECT_FALSE((pM->EnterMessageLoop(&s)).IsSuccess());
	
	delete pM;
}
