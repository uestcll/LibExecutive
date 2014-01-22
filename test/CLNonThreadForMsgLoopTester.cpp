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
static const char *test1_pipe_namepath = "/tmp/test1_for_CLNonThreadForMsgLoop_PrivateQueueForSelfPostMsg";

static int g_for_on1 = 0;
static int g_for_on2 = 0;

static int count = 100000;

static bool bflagdestroy = false;

class CLObserverForCLNonThreadStressTest : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		EXPECT_EQ((long)pContext, 2);

		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLObserverForCLNonThreadStressTest::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLObserverForCLNonThreadStressTest::On_2));

		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, new CLMsg1ForCLNonThreadForMsgLoopTest, bflagdestroy).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLNonThreadForMsgLoopTest *p = dynamic_cast<CLMsg1ForCLNonThreadForMsgLoopTest*>(pm);
		EXPECT_TRUE(p != 0);

		CLMessagePoster *poster = CLExecutiveNameServer::GetInstance()->GetCommunicationPtr(test_pipe_name);
		EXPECT_TRUE(poster != 0);

		for(int i = 0; i < count; i++)
		{
			CLProtocolDataPoster *pd = new CLProtocolDataPoster();
			CLStatus s = poster->PostMessage(new CLMsg2ForCLNonThreadForMsgLoopTest, new CLDataPostResultNotifier(bflagdestroy), pd);
			if(!s.IsSuccess())
			{
				EXPECT_TRUE(s.m_clErrorCode != MSG_POST_ERROR);
				
				if(s.m_clErrorCode == MSG_POST_ERROR)
					break;

				while(true)
				{
					CLStatus s1 = poster->PostLeftMessage(pd);
					if(!s1.IsSuccess())
					{
						EXPECT_TRUE(s1.m_clErrorCode != MSG_POST_ERROR);

						if(s1.m_clErrorCode == MSG_POST_ERROR)
							break;

						continue;
					}
					break;
				}
			}
		}

		EXPECT_TRUE(CLExecutiveNameServer::GetInstance()->ReleaseCommunicationPtr(test_pipe_name).IsSuccess());

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

		if(g_for_on2 == count)
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
	EXPECT_EQ(g_for_on2, count);
}

TEST(CLNonThreadForMsgLoop, PRIVATE_PIPE_QUEUE)
{
	g_for_on1 = 0;
	g_for_on2 = 0;

	CLNonThreadForMsgLoop thread(new CLSTLQueue_CLNonThreadForMsgLoop, test_pipe_name, EXECUTIVE_IN_PROCESS_USE_PIPE_QUEUE);
	EXPECT_TRUE(thread.Run((void *)3).IsSuccess());

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, count);
}

TEST(CLNonThreadForMsgLoop, Shared_PIPE_QUEUE)
{
	g_for_on1 = 0;
	g_for_on2 = 0;

	bflagdestroy = true;

	CLNonThreadForMsgLoop thread(new CLSTLQueue_CLNonThreadForMsgLoop, test_pipe_name, EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE);

	EXPECT_TRUE(thread.RegisterSerializer(1, new CLMsg1ForCLNonThreadForMsgLoopTest_Serializer).IsSuccess());
	EXPECT_TRUE(thread.RegisterSerializer(2, new CLMsg2ForCLNonThreadForMsgLoopTest_Serializer).IsSuccess());

	EXPECT_TRUE(thread.RegisterDeserializer(1, new CLMsg1ForCLNonThreadForMsgLoopTest_Deserializer).IsSuccess());
	EXPECT_TRUE(thread.RegisterDeserializer(2, new CLMsg2ForCLNonThreadForMsgLoopTest_Deserializer).IsSuccess());

	EXPECT_TRUE(thread.Run((void *)3).IsSuccess());

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, count);
}

class CLStressObserverForCLNonThreadForMsgLoop_Process : public CLMessageObserver
{
public:
	CLProcess *process;

	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		EXPECT_EQ((long)pContext, 3);

		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLStressObserverForCLNonThreadForMsgLoop_Process::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLStressObserverForCLNonThreadForMsgLoop_Process::On_2));

		process = new CLProcess(new CLProcessFunctionForExec(), true);
		EXPECT_TRUE((process->Run((void *)"../test_for_exec/test_for_CLNonThreadForMsgLoop_Stress_Testing/main")).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLNonThreadForMsgLoopTest *p = dynamic_cast<CLMsg1ForCLNonThreadForMsgLoopTest*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on1++;

		CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
		EXPECT_TRUE(pNameServer != 0);

		CLEvent *m_pEvent = new CLEvent(test1_pipe_name, true);
		CLMultiMsgSerializer *m_pMsgSerializer = new CLMultiMsgSerializer;
		EXPECT_TRUE(m_pMsgSerializer->RegisterSerializer(1, new CLMsg1ForCLNonThreadForMsgLoopTest_Serializer).IsSuccess());
		EXPECT_TRUE(m_pMsgSerializer->RegisterSerializer(2, new CLMsg2ForCLNonThreadForMsgLoopTest_Serializer).IsSuccess());

		CLMessagePoster *pMsgPoster = new CLMessagePoster(m_pMsgSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(test1_pipe_namepath), m_pEvent);
		EXPECT_TRUE(pMsgPoster->Initialize(new CLInitialDataPostChannelNotifier(), 0).IsSuccess());

		EXPECT_TRUE(pNameServer->Register(test1_pipe_name, pMsgPoster).IsSuccess());

		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test1_pipe_name, new CLMsg1ForCLNonThreadForMsgLoopTest, true).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLNonThreadForMsgLoopTest *p = dynamic_cast<CLMsg2ForCLNonThreadForMsgLoopTest*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on2++;

		if(g_for_on2 == count)
		{
			EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test1_pipe_name, new CLMsg2ForCLNonThreadForMsgLoopTest, true).IsSuccess());

			CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
			EXPECT_TRUE(pNameServer != 0);
			pNameServer->ReleaseCommunicationPtr(test1_pipe_name);

			process->WaitForDeath();

			return CLStatus(QUIT_MESSAGE_LOOP, 0);
		}

		return CLStatus(0, 0);
	}
};

TEST(CLNonThreadForMsgLoop, SHARED_PIPE_QUEUE_STRESS)
{
	g_for_on1 = 0;
	g_for_on2 = 0;

	CLNonThreadForMsgLoop thread(new CLStressObserverForCLNonThreadForMsgLoop_Process, test_pipe_name, EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE);

	EXPECT_TRUE(thread.RegisterSerializer(1, new CLMsg1ForCLNonThreadForMsgLoopTest_Serializer).IsSuccess());
	EXPECT_TRUE(thread.RegisterSerializer(2, new CLMsg2ForCLNonThreadForMsgLoopTest_Serializer).IsSuccess());

	EXPECT_TRUE(thread.RegisterDeserializer(1, new CLMsg1ForCLNonThreadForMsgLoopTest_Deserializer).IsSuccess());
	EXPECT_TRUE(thread.RegisterDeserializer(2, new CLMsg2ForCLNonThreadForMsgLoopTest_Deserializer).IsSuccess());

	EXPECT_TRUE(thread.Run((void *)3).IsSuccess());

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, count);
}