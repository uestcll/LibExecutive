#include <gtest/gtest.h>
#include <unistd.h>
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

		for(int i = 0; i < 4096 * 2/8; i++)
		{
			EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("CLMsgLoopManagerForSTLqueueTester", new CLMessage(i+1)).IsSuccess());
		}

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

TEST(CLMsgLoopManagerForSTLqueue, Normal)
{
	CLLogger::WriteLogMsg("CLMsgLoopManagerForSTLqueue Test", 0);

	CLMessageLoopManager *pM = new CLMsgLoopManagerForSTLqueue(new CLObserverTesterForCLMsgLoopManagerForSTLqueue, "CLMsgLoopManagerForSTLqueueTester");

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	EXPECT_TRUE(pM->EnterMessageLoop(&s).IsSuccess());
	
	delete pM;

	EXPECT_TRUE(g1);
	EXPECT_TRUE(g2);
	EXPECT_TRUE(g3);
}

void *TestThreadForCLMsgLoopManagerForSTLqueue(void *)
{
	sleep(5);

	CLMessageLoopManager *pM = new CLMsgLoopManagerForSTLqueue(new CLObserverTesterForCLMsgLoopManagerForSTLqueue, "CLMsgLoopManagerForSTLqueueTester");

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	CLLogger::WriteLogMsg("The Following bug is produced on purpose 3", 0);
	EXPECT_FALSE(pM->EnterMessageLoop(&s).IsSuccess());

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	EXPECT_FALSE(pM->EnterMessageLoop(&s).IsSuccess());

	delete pM;

	for(int i = 0; i < 3; i++)
	{
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("CLMsgLoopManagerForSTLqueueTester", new CLMessage(i+1)).IsSuccess());
	}

	return 0;
}

class CLObserverTesterForCLMsgLoopManagerForSTLqueue1 : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLObserverTesterForCLMsgLoopManagerForSTLqueue::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLObserverTesterForCLMsgLoopManagerForSTLqueue::On_2));
		pMessageLoop->Register(3, (CallBackForMessageLoop)(&CLObserverTesterForCLMsgLoopManagerForSTLqueue::On_3));

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

TEST(CLMsgLoopManagerForSTLqueue, MultiRegister)
{
	g1 = false;
	g2 = false;
	g3 = false;

	CLMessageLoopManager *pM = new CLMsgLoopManagerForSTLqueue(new CLObserverTesterForCLMsgLoopManagerForSTLqueue1, "CLMsgLoopManagerForSTLqueueTester");

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	pthread_t tid;
	pthread_create(&tid, 0, TestThreadForCLMsgLoopManagerForSTLqueue, 0);

	EXPECT_TRUE(pM->EnterMessageLoop(&s).IsSuccess());

	delete pM;

	pthread_join(tid, 0);

	EXPECT_TRUE(g1);
	EXPECT_TRUE(g2);
	EXPECT_TRUE(g3);
}

class CLObserverTester1ForCLMsgLoopManagerForSTLqueue : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		return CLStatus(-1, 0);
	}
};

TEST(CLMsgLoopManagerForSTLqueue, ObserverInitializeFailure)
{
	CLMessageLoopManager *pM = new CLMsgLoopManagerForSTLqueue(new CLObserverTester1ForCLMsgLoopManagerForSTLqueue, "CLMsgLoopManagerForSTLqueueTester");

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	EXPECT_FALSE((pM->EnterMessageLoop(&s)).IsSuccess());

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	EXPECT_FALSE((pM->EnterMessageLoop(&s)).IsSuccess());
	
	delete pM;
}