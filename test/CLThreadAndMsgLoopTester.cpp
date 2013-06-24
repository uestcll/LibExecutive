#include <gtest/gtest.h>
#include "CLThread.h"
#include "CLMessageObserver.h"
#include "CLMessageLoopManager.h"
#include "CLMessageQueueBySTLqueue.h"
#include "CLExecutiveFunctionForMsgLoop.h"
#include "CLMsgLoopManagerForSTLqueue.h"
#include "CLMessage.h"
#include "CLExecutiveNameServer.h"
#include "CLExecutiveCommunication.h"
#include <unistd.h>
#include "CLThreadInitialFinishedNotifier.h"

bool g_bTAMLT1 = false;
bool g_bTAMLT2 = false;
bool g_bTAMLT3 = false;

class CLObserverTesterForThreadAndMsgLoop: public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLObserverTesterForThreadAndMsgLoop::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLObserverTesterForThreadAndMsgLoop::On_2));
		pMessageLoop->Register(3, (CallBackForMessageLoop)(&CLObserverTesterForThreadAndMsgLoop::On_3));

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		EXPECT_EQ(pm->m_clMsgID, 1);
		g_bTAMLT1 = true;
		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		EXPECT_EQ(pm->m_clMsgID, 2);
		g_bTAMLT2 = true;
		return CLStatus(0, 0);
	}

	CLStatus On_3(CLMessage *pm)
	{
		EXPECT_EQ(pm->m_clMsgID, 3);
		g_bTAMLT3 = true;
		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

bool g_bTAMLT4 = false;

class CLMsgForTAM : public CLMessage
{
public:
	CLMsgForTAM() : CLMessage(5)
	{
	}

	virtual ~CLMsgForTAM()
	{
		g_bTAMLT4 = true;
	}
};



TEST(CLThreadAndMsgLoopTester, Normal)
{
	CLExecutive *pExecutive = new CLThread(new CLExecutiveFunctionForMsgLoop(new CLMsgLoopManagerForSTLqueue(new CLObserverTesterForThreadAndMsgLoop, "CLThreadAndMsgLoopTester")), true);

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	EXPECT_TRUE((pExecutive->Run(&s)).IsSuccess());

	sleep(2);

	CLExecutiveCommunication *pq = (CLExecutiveNameServer::GetInstance())->GetCommunicationPtr("CLThreadAndMsgLoopTester");

	EXPECT_TRUE((pq->PostExecutiveMessage(new CLMessage(1))).IsSuccess());
	EXPECT_TRUE((pq->PostExecutiveMessage(new CLMessage(2))).IsSuccess());
	EXPECT_TRUE((pq->PostExecutiveMessage(new CLMessage(3))).IsSuccess());
	EXPECT_TRUE((pq->PostExecutiveMessage(new CLMsgForTAM())).IsSuccess());

	EXPECT_TRUE(((CLExecutiveNameServer::GetInstance())->ReleaseCommunicationPtr("CLThreadAndMsgLoopTester")).IsSuccess());

	pExecutive->WaitForDeath();

	EXPECT_TRUE(g_bTAMLT1);
	EXPECT_TRUE(g_bTAMLT2);
	EXPECT_TRUE(g_bTAMLT3);
	EXPECT_TRUE(g_bTAMLT4);
}

TEST(CLThreadAndMsgLoopTester, Normal2)
{
	g_bTAMLT1 = false;
	g_bTAMLT2 = false;
	g_bTAMLT3 = false;
	g_bTAMLT4 = false;
	
	CLExecutive *pExecutive = new CLThread(new CLExecutiveFunctionForMsgLoop(new CLMsgLoopManagerForSTLqueue(new CLObserverTesterForThreadAndMsgLoop, "CLThreadAndMsgLoopTester")));

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	EXPECT_TRUE((pExecutive->Run(&s)).IsSuccess());

	sleep(2);

	CLExecutiveCommunication *pq = (CLExecutiveNameServer::GetInstance())->GetCommunicationPtr("CLThreadAndMsgLoopTester");

	EXPECT_TRUE((pq->PostExecutiveMessage(new CLMessage(1))).IsSuccess());
	EXPECT_TRUE((pq->PostExecutiveMessage(new CLMessage(2))).IsSuccess());
	EXPECT_TRUE((pq->PostExecutiveMessage(new CLMessage(3))).IsSuccess());
	EXPECT_TRUE((pq->PostExecutiveMessage(new CLMsgForTAM())).IsSuccess());

	sleep(2);

	EXPECT_TRUE(((CLExecutiveNameServer::GetInstance())->ReleaseCommunicationPtr("CLThreadAndMsgLoopTester")).IsSuccess());

	EXPECT_TRUE(g_bTAMLT1);
	EXPECT_TRUE(g_bTAMLT2);
	EXPECT_TRUE(g_bTAMLT3);
	EXPECT_TRUE(g_bTAMLT4);
}

