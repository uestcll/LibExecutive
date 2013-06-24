#include <gtest/gtest.h>
#include "LibExecutive.h"

TEST(CLNonThreadForMsgLoop, Constructor)
{
	try
	{
		CLNonThreadForMsgLoop t(0, 0);
	}
	catch(...)
	{
		EXPECT_TRUE(true);
		return;
	}

	EXPECT_TRUE(false);
}

TEST(CLNonThreadForMsgLoop, Constructor1)
{
	try
	{
		CLNonThreadForMsgLoop t(0, "ds");
	}
	catch(...)
	{
		EXPECT_TRUE(true);
		return;
	}

	EXPECT_TRUE(false);
}

TEST(CLNonThreadForMsgLoop, Constructor2)
{
	try
	{
		CLNonThreadForMsgLoop t(0, "");
	}
	catch(...)
	{
		EXPECT_TRUE(true);
		return;
	}

	EXPECT_TRUE(false);
}

TEST(CLNonThreadForMsgLoop, Constructor3)
{
	try
	{
		CLNonThreadForMsgLoop t((CLMessageObserver *)1, 0);
	}
	catch(...)
	{
		EXPECT_TRUE(true);
		return;
	}

	EXPECT_TRUE(false);
}

TEST(CLNonThreadForMsgLoop, Constructor4)
{
	try
	{
		CLNonThreadForMsgLoop t((CLMessageObserver *)1, "");
	}
	catch(...)
	{
		EXPECT_TRUE(true);
		return;
	}

	EXPECT_TRUE(false);
}

static bool g_b1 = false;
static bool g_b2 = false;
static bool g_b3 = false;
static bool g_b4 = false;
static bool g_b5 = false;

class CLChildObserverForNonProxyTester : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		EXPECT_EQ(1, (long)pContext);

		g_b1 = true;

		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLChildObserverForNonProxyTester::On_1));

		EXPECT_TRUE((CLExecutiveNameServer::PostExecutiveMessage("test_1_2_3", new CLMessage(1))).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		g_b2 = true;
		EXPECT_EQ(pm->m_clMsgID, 1);
		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};


class CLTestForCLNonThreadForMsgLoop : public CLMessageObserver
{
	CLThreadForMsgLoop *m_p1;

public:
	CLTestForCLNonThreadForMsgLoop()
	{
	}

	virtual ~CLTestForCLNonThreadForMsgLoop()
	{
		g_b3 = true;
		delete m_p1;
	}

	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		g_b4 = true;
		EXPECT_EQ(2, (long)pContext);

		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLTestForCLNonThreadForMsgLoop::On_1));

		m_p1 = new CLThreadForMsgLoop(new CLChildObserverForNonProxyTester, "test_4_5_6", true);

		EXPECT_TRUE((m_p1->Run((void *)1)).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		g_b5 = true;
		EXPECT_EQ(pm->m_clMsgID, 1);
		CLExecutiveNameServer::PostExecutiveMessage("test_4_5_6", new CLMessage(1));
		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

TEST(CLNonThreadForMsgLoop, Normal)
{
	{
		CLNonThreadForMsgLoop p(new CLTestForCLNonThreadForMsgLoop, "test_1_2_3");

		p.Run((void *)2);
	}

	EXPECT_TRUE(g_b1);
	EXPECT_TRUE(g_b2);
	EXPECT_TRUE(g_b3);
	EXPECT_TRUE(g_b4);
	EXPECT_TRUE(g_b5);
}