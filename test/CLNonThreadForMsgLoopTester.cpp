#include <gtest/gtest.h>
#include "LibExecutive.h"

class CLMsg1ForCLNonThreadForMsgLoopTest : public CLMessage
{
public:
	CLMsg1ForCLNonThreadForMsgLoopTest() : CLMessage(1)
	{
		i = 2;
		j = 3;
	}

	virtual ~CLMsg1ForCLNonThreadForMsgLoopTest()
	{
	}

	int i;
	int j;
};

class CLMsg2ForCLNonThreadForMsgLoopTest : public CLMessage
{
public:
	CLMsg2ForCLNonThreadForMsgLoopTest() : CLMessage(2)
	{
	}

	virtual ~CLMsg2ForCLNonThreadForMsgLoopTest()
	{
	}
};

class CLMsg1ForCLNonThreadForMsgLoopTest_Serializer : public CLMessageSerializer
{
public:
	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVectors *pIOVectors)
	{
		CLMsg1ForCLNonThreadForMsgLoopTest *pm = dynamic_cast<CLMsg1ForCLNonThreadForMsgLoopTest *>(pMsg);
		EXPECT_TRUE(pm != 0);

		char *p = new char [20];

		int *pHead = (int *)p;
		*pHead = 16;

		long *pid = (long *)(p + 4);
		*pid = pm->m_clMsgID;

		int *pi = (int *)(p + 12);
		*pi = pm->i;

		int *pj = (int *)(p + 16);
		*pj = pm->j;

		EXPECT_TRUE(pIOVectors->PushBack(p, 20, true).IsSuccess());

		return CLStatus(0, 0);
	}

	virtual ~CLMsg1ForCLNonThreadForMsgLoopTest_Serializer()
	{
	}
};

class CLMsg2ForCLNonThreadForMsgLoopTest_Serializer : public CLMessageSerializer
{
public:
	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVectors *pIOVectors)
	{
		CLMsg2ForCLNonThreadForMsgLoopTest *pm = dynamic_cast<CLMsg2ForCLNonThreadForMsgLoopTest *>(pMsg);
		EXPECT_TRUE(pm != 0);

		char *p = new char [12];

		int *pHead = (int *)p;
		*pHead = 8;

		long *pid = (long *)(p + 4);
		*pid = pm->m_clMsgID;

		EXPECT_TRUE(pIOVectors->PushBack(p, 12, true).IsSuccess());

		return CLStatus(0, 0);
	}

	virtual ~CLMsg2ForCLNonThreadForMsgLoopTest_Serializer()
	{
	}
};

class CLMsg1ForCLNonThreadForMsgLoopTest_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLStatus Deserialize(CLIOVectors& IOVectors, CLMessage **ppMsg, CLBufferManager& BufferManager)
	{
		CLIteratorForIOVectors iter;
		IOVectors.GetIterator(0, iter);

		int len = 0;
		EXPECT_TRUE(IOVectors.ReadBlock(iter, (char *)(&len), 4).IsSuccess());
		EXPECT_EQ(len, 16);

		long id = 0;
		EXPECT_TRUE(IOVectors.ReadBlock(iter, (char *)(&id), 8).IsSuccess());
		EXPECT_EQ(id, 1);

		int x = 0;
		EXPECT_TRUE(IOVectors.ReadBlock(iter, (char *)(&x), 4).IsSuccess());
		EXPECT_EQ(x, 2);

		int y = 0;
		EXPECT_TRUE(IOVectors.ReadBlock(iter, (char *)(&y), 4).IsSuccess());
		EXPECT_EQ(y, 3);

		*ppMsg = new CLMsg1ForCLNonThreadForMsgLoopTest();

		return CLStatus(0, 0);
	}

	virtual ~CLMsg1ForCLNonThreadForMsgLoopTest_Deserializer()
	{
	}
};

class CLMsg2ForCLNonThreadForMsgLoopTest_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLStatus Deserialize(CLIOVectors& IOVectors, CLMessage **ppMsg, CLBufferManager& BufferManager)
	{
		CLIteratorForIOVectors iter;
		IOVectors.GetIterator(0, iter);

		int len = 0;
		EXPECT_TRUE(IOVectors.ReadBlock(iter, (char *)(&len), 4).IsSuccess());
		EXPECT_EQ(len, 8);

		long id = 0;
		EXPECT_TRUE(IOVectors.ReadBlock(iter, (char *)(&id), 8).IsSuccess());
		EXPECT_EQ(id, 2);

		*ppMsg = new CLMsg2ForCLNonThreadForMsgLoopTest();

		return CLStatus(0, 0);
	}

	virtual ~CLMsg2ForCLNonThreadForMsgLoopTest_Deserializer()
	{
	}
};

static const char *test_pipe_name = "test_for_CLNonThreadForMsgLoop_PrivateQueueForSelfPostMsg";
static const char *test1_pipe_name = "test1_for_CLNonThreadForMsgLoop_PrivateQueueForSelfPostMsg";

static int g_for_on1 = 0;
static int g_for_on2 = 0;

class CLObserverForCLNonThreadStressTest : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		EXPECT_EQ((long)pContext, 2);

		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLObserverForCLNonThreadStressTest::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLObserverForCLNonThreadStressTest::On_2));

		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, new CLMsg1ForCLNonThreadForMsgLoopTest).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLNonThreadForMsgLoopTest *p = dynamic_cast<CLMsg1ForCLNonThreadForMsgLoopTest*>(pm);
		EXPECT_TRUE(p != 0);

		for(int i = 0; i < 1000000; i++)
		{
			EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, new CLMsg2ForCLNonThreadForMsgLoopTest).IsSuccess());
		}

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLNonThreadForMsgLoopTest *p = dynamic_cast<CLMsg2ForCLNonThreadForMsgLoopTest*>(pm);
		EXPECT_TRUE(p != 0);

		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

class CLSTLQueue_CLNonThreadForMsgLoop : public CLMessageObserver
{
public:
	CLThreadForMsgLoop *m_pThread;

	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		EXPECT_EQ((long)pContext, 3);

		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLSTLQueue_CLNonThreadForMsgLoop::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLSTLQueue_CLNonThreadForMsgLoop::On_2));

		m_pThread = new CLThreadForMsgLoop(new CLObserverForCLNonThreadStressTest, test1_pipe_name, true);
		EXPECT_TRUE(m_pThread->Run((void *)2).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLNonThreadForMsgLoopTest *p = dynamic_cast<CLMsg1ForCLNonThreadForMsgLoopTest*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on1++;

		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test1_pipe_name, new CLMsg1ForCLNonThreadForMsgLoopTest).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLNonThreadForMsgLoopTest *p = dynamic_cast<CLMsg2ForCLNonThreadForMsgLoopTest*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on2++;

		if(g_for_on2 == 1000000)
		{
			EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test1_pipe_name, new CLMsg2ForCLNonThreadForMsgLoopTest).IsSuccess());

			delete m_pThread;

			return CLStatus(QUIT_MESSAGE_LOOP, 0);
		}

		return CLStatus(0, 0);
	}
};

TEST(CLNonThreadForMsgLoop, STL_QUEUE)
{
	CLLogger::WriteLogMsg("CLNonThreadForMsgLoop Test", 0);

	CLNonThreadForMsgLoop thread(new CLSTLQueue_CLNonThreadForMsgLoop, test_pipe_name, EXECUTIVE_IN_PROCESS_USE_STL_QUEUE);
	EXPECT_TRUE(thread.Run((void *)3).IsSuccess());

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, 1000000);
}

/*
static const int count = 1000000;

class CLStressTester : public CLExecutiveFunctionProvider
{
public:
	virtual CLStatus RunExecutiveFunction(void* pContext)
	{
		for(int i = 0; i < count; i++)
		{
			CLStatus s = CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, new CLMsg1ForCLNonThreadForMsgLoop);
			if(!s.IsSuccess())
			{
				sleep(1);
				CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, new CLMsg1ForCLNonThreadForMsgLoop);
			}
		}
	}
};

class CLStressObserverForCLNonThreadForMsgLoop : public CLMessageObserver
{
	CLThread *pthread;

public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLStressObserverForCLNonThreadForMsgLoop::On_1));

		pthread = new CLThread(new CLStressTester, true);
		EXPECT_TRUE(pthread->Run().IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLNonThreadForMsgLoop *p = dynamic_cast<CLMsg1ForCLNonThreadForMsgLoop*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on1++;

		if(g_for_on1 == count)
		{
			pthread->WaitForDeath();
			return CLStatus(QUIT_MESSAGE_LOOP, 0);
		}
		else
			return CLStatus(0, 0);
	}
};

TEST(CLNonThreadForMsgLoop, STL_QUEUE_STRESS)
{
	g_for_on1 = 0;
	g_for_msg1 = 0;

	CLNonThreadForMsgLoop thread(new CLStressObserverForCLNonThreadForMsgLoop, test_pipe_name, EXECUTIVE_IN_PROCESS_USE_STL_QUEUE);
	EXPECT_TRUE(thread.Run(0).IsSuccess());

	EXPECT_EQ(g_for_on1, count);
	EXPECT_EQ(g_for_msg1, count);
}

TEST(CLNonThreadForMsgLoop, PRIVATE_PIPE_QUEUE_STRESS)
{
	g_for_on1 = 0;
	g_for_msg1 = 0;

	CLNonThreadForMsgLoop thread(new CLStressObserverForCLNonThreadForMsgLoop, test_pipe_name, EXECUTIVE_IN_PROCESS_USE_PIPE_QUEUE);
	EXPECT_TRUE(thread.Run(0).IsSuccess());

	EXPECT_EQ(g_for_on1, count);
	EXPECT_EQ(g_for_msg1, count);
}

class CLStressObserverForCLNonThreadForMsgLoop_Process : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLStressObserverForCLNonThreadForMsgLoop_Process::On_1));

		CLProcess *process = new CLProcess(new CLProcessFunctionForExec, false);
		EXPECT_TRUE((process->Run((void *)"../test_for_exec/test_for_CLNonThreadForMsgLoop_Stress_Testing/main")).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLNonThreadForMsgLoop *p = dynamic_cast<CLMsg1ForCLNonThreadForMsgLoop*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on1++;

		if(g_for_on1 == count)
			return CLStatus(QUIT_MESSAGE_LOOP, 0);
		else
			return CLStatus(0, 0);
	}
};

TEST(CLNonThreadForMsgLoop, SHARED_PIPE_QUEUE_STRESS)
{
	g_for_on1 = 0;
	g_for_msg1 = 0;

	{
		CLNonThreadForMsgLoop thread(new CLStressObserverForCLNonThreadForMsgLoop_Process, test_pipe_name, EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE);
		EXPECT_TRUE(thread.RegisterDeserializer(1, new CLMsg1ForCLNonThreadForMsgLoop_Deserializer).IsSuccess());
		EXPECT_TRUE(thread.Run(0).IsSuccess());
	}

	EXPECT_EQ(g_for_on1, count);
	EXPECT_EQ(g_for_msg1, count);
}*/