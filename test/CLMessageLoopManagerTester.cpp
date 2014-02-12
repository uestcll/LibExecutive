#include <gtest/gtest.h>
#include "LibExecutive.h"

static const int ADD_MSG_ID = 1; 

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
	}

	int m_i;
	int m_j;
};

class TestObserverForCLMessageLoopManager : public CLMessageObserver
{
public:
	TestObserverForCLMessageLoopManager()
	{
		j = 0;
	}

	virtual ~TestObserverForCLMessageLoopManager()
	{
	}

	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		if(j != 0)
		{
			return CLStatus(-1, NORMAL_ERROR);
		}

		EXPECT_FALSE(pMessageLoop->Register(ADD_MSG_ID, 0).IsSuccess());
		EXPECT_TRUE(pMessageLoop->Register(ADD_MSG_ID, (CallBackForMessageLoop)(&TestObserverForCLMessageLoopManager::On_Add)).IsSuccess());
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

		if(i == 2)
			return CLStatus(QUIT_MESSAGE_LOOP, 0);
		else
			return CLStatus(0, 0);
	}

	int j;
};

class TestClassForCLMessageLoopManager : public CLMessageLoopManager
{
public:
	TestClassForCLMessageLoopManager(CLMessageObserver *pMessageObserver) : CLMessageLoopManager(pMessageObserver)
	{
		i = 0;
		j = 0;
		p = 0;
	}

	virtual ~TestClassForCLMessageLoopManager()
	{
	}

	virtual CLStatus Initialize()
	{
		if(i == 0)
			return CLStatus(0, 0);
		else
			return CLStatus(-1, NORMAL_ERROR);
	}

	virtual CLStatus Uninitialize()
	{
		if(j == 0)
			return CLStatus(0, 0);
		else
			return CLStatus(-1, NORMAL_ERROR);
	}

	virtual CLStatus WaitForMessage()
	{
		if(p != 0)
			return CLStatus(-1, NORMAL_ERROR);

		for(int k = 0; k < 10; k++)
			m_MessageContainer.push(new CLTestAddMsgForCLMessageLoopManager(2, 5));

		return CLStatus(0, 0);
	}

	int i;
	int j;
	int p;
};

TEST(CLMessageLoopManager, EnterMessageLoop_Features_Test)
{
	CLLogger::WriteLogMsg("CLMessageLoopManager Test", 0);

	SLExecutiveInitialParameter i;
	i.pContext = 0;
	CLThreadInitialFinishedNotifier j(0);
	i.pNotifier = 0;

	CLMessageLoopManager *p = new TestClassForCLMessageLoopManager(new TestObserverForCLMessageLoopManager);

	EXPECT_FALSE(p->EnterMessageLoop(0).IsSuccess());
	EXPECT_FALSE(p->EnterMessageLoop(&i).IsSuccess());
	i.pNotifier = &j;

	EXPECT_TRUE(p->EnterMessageLoop(&i).IsSuccess());

	delete p;
}

TEST(CLMessageLoopManager, EnterMessageLoop_Failure_Test)
{
	SLExecutiveInitialParameter i;
	i.pContext = 0;
	CLThreadInitialFinishedNotifier j(0);
	i.pNotifier = &j;

	TestObserverForCLMessageLoopManager *q = new TestObserverForCLMessageLoopManager;
	TestClassForCLMessageLoopManager *p = new TestClassForCLMessageLoopManager(q);
	p->i = -1;

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	EXPECT_FALSE(p->EnterMessageLoop(&i).IsSuccess());
	p->i = 0;

	q->j = -1;
	p->j = -1;
	CLLogger::WriteLogMsg("2 The Following bug is produced on purpose 2", 0);
	EXPECT_FALSE(p->EnterMessageLoop(&i).IsSuccess());

	delete p;
}

TEST(CLMessageLoopManager, EnterMessageLoop_WaitForMessageError_Test)
{
	SLExecutiveInitialParameter i;
	i.pContext = 0;
	CLThreadInitialFinishedNotifier j(0);
	i.pNotifier = &j;

	TestObserverForCLMessageLoopManager *q = new TestObserverForCLMessageLoopManager;
	TestClassForCLMessageLoopManager *p = new TestClassForCLMessageLoopManager(q);
	p->p = 1;

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	EXPECT_TRUE(p->EnterMessageLoop(&i).IsSuccess());

	delete p;
}