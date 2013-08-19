#include <gtest/gtest.h>
#include "LibExecutive.h"

static const int ADD_MSG_ID = 1; 

static bool gformsg = false;
static bool gfordes = false;
static bool gfordes2 = false;
static int gformsgdes = 0;
static bool gforinit = false;
static bool gforuninit = false;

class CLTestAddMsgForCLMessageLoopManager : public CLMessage
{
public:
	CLTestAddMsgForCLMessageLoopManager(int i, int j) : CLMessage(ADD_MSG_ID)
	{
		m_i = i;
		m_j = j;
	}

	virtual ~CLTestAddMsgForCLMessageLoopManager()
	{
		gformsgdes++;
	}

	int m_i;
	int m_j;
};

class TestObserverForCLMessageLoopManager : public CLMessageObserver
{
public:
	TestObserverForCLMessageLoopManager()
	{}

	virtual ~TestObserverForCLMessageLoopManager()
	{
		gfordes2 = true;
	}

	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(ADD_MSG_ID, (CallBackForMessageLoop)(&TestObserverForCLMessageLoopManager::On_Add));
		return CLStatus(0, 0);
	}

	CLStatus On_Add(CLMessage* pMsg)
	{
		static int i = 0;

		CLTestAddMsgForCLMessageLoopManager *p = dynamic_cast<CLTestAddMsgForCLMessageLoopManager *>(pMsg);
		EXPECT_TRUE(p != NULL);

		EXPECT_TRUE(p->m_i == 2);
		EXPECT_TRUE(p->m_j == 5);

		i++;

		gformsg = true;

		if(i == 2)
			return CLStatus(QUIT_MESSAGE_LOOP, 0);
		else
			return CLStatus(0, 0);
	}
};

class TestClassForCLMessageLoopManager : public CLMessageLoopManager
{
public:
	TestClassForCLMessageLoopManager(CLMessageObserver *pMessageObserver) : CLMessageLoopManager(pMessageObserver)
	{}

	virtual ~TestClassForCLMessageLoopManager()
	{
		gfordes = true;
	}

	virtual CLStatus Initialize()
	{
		gforinit = true;
		return CLStatus(0, 0);
	}

	virtual CLStatus Uninitialize()
	{
		gforuninit = true;
		return CLStatus(0, 0);
	}

	virtual CLMessage* WaitForMessage()
	{
		return new CLTestAddMsgForCLMessageLoopManager(2, 5);
	}
};

TEST(CLMessageLoopManager, Normal)
{
	SLExecutiveInitialParameter i;
	i.pContext = NULL;

	CLThreadInitialFinishedNotifier j(0);
	i.pNotifier = &j;

	CLMessageLoopManager *p = new TestClassForCLMessageLoopManager(new TestObserverForCLMessageLoopManager);

	EXPECT_FALSE(p->EnterMessageLoop(0).IsSuccess());

	p->EnterMessageLoop(&i);

	delete p;

	EXPECT_EQ(gformsgdes, 2);
	EXPECT_TRUE(gfordes);
	EXPECT_TRUE(gfordes2);
	EXPECT_TRUE(gformsg);
	EXPECT_TRUE(gforinit);
	EXPECT_TRUE(gforuninit);
}