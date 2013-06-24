#include <gtest/gtest.h>
#include "LibExecutive.h"

TEST(CLThreadForMsgLoop, Constructor)
{
	try
	{
		CLThreadForMsgLoop t(0, 0);
	}
	catch(...)
	{
		EXPECT_TRUE(true);
		return;
	}

	EXPECT_TRUE(false);
}

TEST(CLThreadForMsgLoop, Constructor1)
{
	try
	{
		CLThreadForMsgLoop t(0, "ds");
	}
	catch(...)
	{
		EXPECT_TRUE(true);
		return;
	}

	EXPECT_TRUE(false);
}

TEST(CLThreadForMsgLoop, Constructor2)
{
	try
	{
		CLThreadForMsgLoop t(0, "");
	}
	catch(...)
	{
		EXPECT_TRUE(true);
		return;
	}

	EXPECT_TRUE(false);
}

TEST(CLThreadForMsgLoop, Constructor3)
{
	try
	{
		CLThreadForMsgLoop t((CLMessageObserver *)1, 0);
	}
	catch(...)
	{
		EXPECT_TRUE(true);
		return;
	}

	EXPECT_TRUE(false);
}

TEST(CLThreadForMsgLoop, Constructor4)
{
	try
	{
		CLThreadForMsgLoop t((CLMessageObserver *)1, "");
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

class CLTestForCLThreadForMsgLoop : public CLMessageObserver
{
public:
	CLTestForCLThreadForMsgLoop()
	{
	}

	virtual ~CLTestForCLThreadForMsgLoop()
	{
	}

	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		long i = (long)pContext;
		EXPECT_EQ(i, 2);

		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLTestForCLThreadForMsgLoop::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLTestForCLThreadForMsgLoop::On_2));
		pMessageLoop->Register(3, (CallBackForMessageLoop)(&CLTestForCLThreadForMsgLoop::On_3));

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		EXPECT_EQ(pm->m_clMsgID, 1);
		g_b1 = true;
		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		EXPECT_EQ(pm->m_clMsgID, 2);
		g_b2 = true;
		return CLStatus(0, 0);
	}

	CLStatus On_3(CLMessage *pm)
	{
		EXPECT_EQ(pm->m_clMsgID, 3);
		g_b3 = true;
		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

TEST(CLThreadForMsgLoop, Normal)
{
	CLThreadForMsgLoop *p = new CLThreadForMsgLoop(new CLTestForCLThreadForMsgLoop, "testforthreadformsgloop", true);
	p->Run((void*)2);

	CLExecutiveNameServer::PostExecutiveMessage("testforthreadformsgloop", new CLMessage(1));
	CLExecutiveNameServer::PostExecutiveMessage("testforthreadformsgloop", new CLMessage(2));
	CLExecutiveNameServer::PostExecutiveMessage("testforthreadformsgloop", new CLMessage(3));

	delete p;

	EXPECT_TRUE(g_b1);
	EXPECT_TRUE(g_b2);
	EXPECT_TRUE(g_b3);
}