#include <gtest/gtest.h>
#include "LibExecutive.h"

class CLMsg1ForCLThreadForMsgLoopTest : public CLMessage
{
public:
	CLMsg1ForCLThreadForMsgLoopTest() : CLMessage(1)
	{
		i = 2;
		j = 3;
	}

	virtual ~CLMsg1ForCLThreadForMsgLoopTest()
	{
	}

	int i;
	int j;
};

class CLMsg2ForCLThreadForMsgLoopTest : public CLMessage
{
public:
	CLMsg2ForCLThreadForMsgLoopTest() : CLMessage(2)
	{
	}

	virtual ~CLMsg2ForCLThreadForMsgLoopTest()
	{
	}
};

class CLMsg1ForCLThreadForMsgLoopTest_Serializer : public CLMessageSerializer
{
public:
	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVectors *pIOVectors)
	{
		CLMsg1ForCLThreadForMsgLoopTest *pm = dynamic_cast<CLMsg1ForCLThreadForMsgLoopTest *>(pMsg);
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

	virtual ~CLMsg1ForCLThreadForMsgLoopTest_Serializer()
	{
	}
};

class CLMsg2ForCLThreadForMsgLoopTest_Serializer : public CLMessageSerializer
{
public:
	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVectors *pIOVectors)
	{
		CLMsg2ForCLThreadForMsgLoopTest *pm = dynamic_cast<CLMsg2ForCLThreadForMsgLoopTest *>(pMsg);
		EXPECT_TRUE(pm != 0);

		char *p = new char [12];

		int *pHead = (int *)p;
		*pHead = 8;

		long *pid = (long *)(p + 4);
		*pid = pm->m_clMsgID;

		EXPECT_TRUE(pIOVectors->PushBack(p, 12, true).IsSuccess());

		return CLStatus(0, 0);
	}

	virtual ~CLMsg2ForCLThreadForMsgLoopTest_Serializer()
	{
	}
};

class CLMsg1ForCLThreadForMsgLoopTest_Deserializer : public CLMessageDeserializer
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

		*ppMsg = new CLMsg1ForCLThreadForMsgLoopTest();

		return CLStatus(0, 0);
	}

	virtual ~CLMsg1ForCLThreadForMsgLoopTest_Deserializer()
	{
	}
};

class CLMsg2ForCLThreadForMsgLoopTest_Deserializer : public CLMessageDeserializer
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

		*ppMsg = new CLMsg2ForCLThreadForMsgLoopTest();

		return CLStatus(0, 0);
	}

	virtual ~CLMsg2ForCLThreadForMsgLoopTest_Deserializer()
	{
	}
};

static const char *test_pipe_name = "test_for_CLThreadForMsgLoop_PrivateQueueForSelfPostMsg";

static int g_for_on1 = 0;
static int g_for_on2 = 0;

class CLSTLQueue_CLThreadForMsgLoop : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		EXPECT_EQ((long)pContext, 3);

		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLSTLQueue_CLThreadForMsgLoop::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLSTLQueue_CLThreadForMsgLoop::On_2));

		CLMessage *p = new CLMsg1ForCLThreadForMsgLoopTest;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p).IsSuccess());

		p = new CLMsg2ForCLThreadForMsgLoopTest;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLThreadForMsgLoopTest *p = dynamic_cast<CLMsg1ForCLThreadForMsgLoopTest*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on1++;

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLThreadForMsgLoopTest *p = dynamic_cast<CLMsg2ForCLThreadForMsgLoopTest*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on2++;

		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

TEST(CLThreadForMsgLoop, STL_QUEUE)
{
	CLLogger::WriteLogMsg("CLThreadForMsgLoop Test", 0);

	{
		CLThreadForMsgLoop thread(new CLSTLQueue_CLThreadForMsgLoop, test_pipe_name, true, EXECUTIVE_IN_PROCESS_USE_STL_QUEUE);
		EXPECT_TRUE(thread.Run((void *)3).IsSuccess());
	}

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, 1);
}

TEST(CLThreadForMsgLoop, PRIVATE_PIPE_QUEUE)
{
	g_for_on1 = 0;
	g_for_on2 = 0;

	{
		CLThreadForMsgLoop thread(new CLSTLQueue_CLThreadForMsgLoop, test_pipe_name, true, EXECUTIVE_IN_PROCESS_USE_PIPE_QUEUE);
		EXPECT_TRUE(thread.Run((void *)3).IsSuccess());
	}

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, 1);
}

class CLPipeQueue_CLThreadForMsgLoop : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		EXPECT_EQ((long)pContext, 3);

		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLPipeQueue_CLThreadForMsgLoop::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLPipeQueue_CLThreadForMsgLoop::On_2));

		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, new CLMsg1ForCLThreadForMsgLoopTest, true).IsSuccess());
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, new CLMsg2ForCLThreadForMsgLoopTest, true).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLThreadForMsgLoopTest *p = dynamic_cast<CLMsg1ForCLThreadForMsgLoopTest*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on1++;

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLThreadForMsgLoopTest *p = dynamic_cast<CLMsg2ForCLThreadForMsgLoopTest*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on2++;

		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

TEST(CLThreadForMsgLoop, Shared_PIPE_QUEUE)
{
	g_for_on1 = 0;
	g_for_on2 = 0;

	{
		CLThreadForMsgLoop thread(new CLPipeQueue_CLThreadForMsgLoop, test_pipe_name, true, EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE);

		EXPECT_TRUE(thread.RegisterSerializer(1, new CLMsg1ForCLThreadForMsgLoopTest_Serializer).IsSuccess());
		EXPECT_TRUE(thread.RegisterSerializer(2, new CLMsg2ForCLThreadForMsgLoopTest_Serializer).IsSuccess());

		EXPECT_TRUE(thread.RegisterDeserializer(1, new CLMsg1ForCLThreadForMsgLoopTest_Deserializer).IsSuccess());
		EXPECT_TRUE(thread.RegisterDeserializer(2, new CLMsg2ForCLThreadForMsgLoopTest_Deserializer).IsSuccess());

		EXPECT_TRUE(thread.Run((void *)3).IsSuccess());
	}

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, 1);
}

class CLFailureObserver_CLThreadForMsgLoop : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		return CLStatus(-1, NORMAL_ERROR);
	}
};

TEST(CLThreadForMsgLoop, ObserverFailure)
{
	CLThreadForMsgLoop thread(new CLFailureObserver_CLThreadForMsgLoop, test_pipe_name, true, EXECUTIVE_IN_PROCESS_USE_STL_QUEUE);
	
	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	EXPECT_FALSE(thread.Run((void *)3).IsSuccess());
}

class CLEncapsulatorForCLThreadForMsgLoop : public CLProtocolEncapsulator
{
public:
	virtual CLStatus Initialize(uuid_t uuidOfCommObj)
	{
		return CLStatus(0, 0);
	}

	virtual CLStatus Uninitialize()
	{
		return CLStatus(0, 0);
	}

	virtual CLStatus Encapsulate(CLIOVectors *pIOVectors)
	{
		for(int i = 0; i < pIOVectors->Size(); i++)
		{
			unsigned char c;
			EXPECT_TRUE(pIOVectors->ReadBlock(i, (char *)&c, 1).IsSuccess());

			c = ~c;

			EXPECT_TRUE(pIOVectors->WriteBlock(i, (char *)&c, 1).IsSuccess());
		}

		return CLStatus(0, 0);
	}
};

class CLDecapsulatorForCLThreadForMsgLoop : public CLProtocolDecapsulator
{
public:
	CLDecapsulatorForCLThreadForMsgLoop(CLProtocolDecapsulator *p) : CLProtocolDecapsulator(p)
	{
	}

	virtual CLStatus Decapsulate(CLIOVectors& IOVectorsForData, unsigned int Length, std::vector<CLIOVectors *>& vSerializedMsgs, CLIOVectors& IOVectorsForPartialData, void *pContext)
	{
		for(int i = 0; i < IOVectorsForData.Size(); i++)
		{
			unsigned char c;
			EXPECT_TRUE(IOVectorsForData.ReadBlock(i, (char *)&c, 1).IsSuccess());

			c = ~c;

			EXPECT_TRUE(IOVectorsForData.WriteBlock(i, (char *)&c, 1).IsSuccess());
		}

		return m_pProtocolDecapsulator->Decapsulate(IOVectorsForData, Length, vSerializedMsgs, IOVectorsForPartialData, pContext);
	}
};

TEST(CLThreadForMsgLoop, EnAndDecapsulator)
{
	g_for_on1 = 0;
	g_for_on2 = 0;

	{
		CLThreadForMsgLoop thread(new CLPipeQueue_CLThreadForMsgLoop, test_pipe_name, true, EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE, new CLEncapsulatorForCLThreadForMsgLoop, new CLDecapsulatorForCLThreadForMsgLoop(new CLProtocolDecapsulatorByDefaultMsgFormat));

		EXPECT_TRUE(thread.RegisterSerializer(1, new CLMsg1ForCLThreadForMsgLoopTest_Serializer).IsSuccess());
		EXPECT_TRUE(thread.RegisterSerializer(2, new CLMsg2ForCLThreadForMsgLoopTest_Serializer).IsSuccess());

		EXPECT_TRUE(thread.RegisterDeserializer(1, new CLMsg1ForCLThreadForMsgLoopTest_Deserializer).IsSuccess());
		EXPECT_TRUE(thread.RegisterDeserializer(2, new CLMsg2ForCLThreadForMsgLoopTest_Deserializer).IsSuccess());

		EXPECT_TRUE(thread.Run((void *)3).IsSuccess());
	}

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, 1);
}