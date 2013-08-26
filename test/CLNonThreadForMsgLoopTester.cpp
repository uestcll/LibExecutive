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

static const char *test_pipe_name = "test_for_CLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg";

static int g_for_on1 = 0;
static int g_for_on2 = 0;
static int g_for_msg1 = 0;
static int g_for_msg2 = 0;
static int g_for_msg1_dese = 0;
static int g_for_msg2_dese = 0;
static int g_for_msg1_se = 0;
static int g_for_msg2_se = 0;

class CLMsg1ForCLNonThreadForMsgLoop : public CLMessage
{
public:
	CLMsg1ForCLNonThreadForMsgLoop() : CLMessage(1)
	{
		i = 2;
		j = 3;
	}

	virtual ~CLMsg1ForCLNonThreadForMsgLoop()
	{
		g_for_msg1++;
	}

	int i;
	int j;
};

class CLMsg2ForCLNonThreadForMsgLoop : public CLMessage
{
public:
	CLMsg2ForCLNonThreadForMsgLoop() : CLMessage(2)
	{
		i = 4;
		j = 6;
	}

	virtual ~CLMsg2ForCLNonThreadForMsgLoop()
	{
		g_for_msg2++;
	}

	long i;
	int j;
};

class CLMsg1ForCLNonThreadForMsgLoop_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		CLMsg1ForCLNonThreadForMsgLoop *p = new CLMsg1ForCLNonThreadForMsgLoop;
		p->i = *((int *)(pBuffer + sizeof(long)));
		p->j = *((int *)(pBuffer + sizeof(long) + sizeof(int)));
		return p;
	}

	virtual ~CLMsg1ForCLNonThreadForMsgLoop_Deserializer()
	{
		g_for_msg1_dese++;
	}
};

class CLMsg2ForCLNonThreadForMsgLoop_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		CLMsg2ForCLNonThreadForMsgLoop *p = new CLMsg2ForCLNonThreadForMsgLoop;
		p->i = *((long *)(pBuffer + sizeof(long)));
		p->j = *((int *)(pBuffer + sizeof(long) + sizeof(long)));
		return p;
	}

	virtual ~CLMsg2ForCLNonThreadForMsgLoop_Deserializer()
	{
		g_for_msg2_dese++;
	}
};

class CLMsg1ForCLNonThreadForMsgLoop_Serializer : public CLMessageSerializer
{
public:
	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength, unsigned int HeadLength)
	{
		CLMsg1ForCLNonThreadForMsgLoop *p = dynamic_cast<CLMsg1ForCLNonThreadForMsgLoop *>(pMsg);
		EXPECT_TRUE(p != 0);

		*pFullLength = HeadLength + 8 + 4 + 4;
		char *pBuf = new char[*pFullLength];

		long *pID = (long *)(pBuf + HeadLength);
		*pID = p->m_clMsgID;

		int *pi = (int *)(pBuf + HeadLength + 8);
		*pi = p->i;

		int *pj = (int *)(pBuf + HeadLength + 8 + 4);
		*pj = p->j;

		return pBuf;
	}

	virtual ~CLMsg1ForCLNonThreadForMsgLoop_Serializer()
	{
		g_for_msg1_se++;
	}
};

class CLMsg2ForCLNonThreadForMsgLoop_Serializer : public CLMessageSerializer
{
public:
	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength, unsigned int HeadLength)
	{
		CLMsg2ForCLNonThreadForMsgLoop *p = dynamic_cast<CLMsg2ForCLNonThreadForMsgLoop *>(pMsg);
		EXPECT_TRUE(p != 0);

		*pFullLength = HeadLength + 8 + 8 + 4;
		char *pBuf = new char[*pFullLength];

		long *pID = (long *)(pBuf + HeadLength);
		*pID = p->m_clMsgID;

		long *pi = (long *)(pBuf + HeadLength + 8);
		*pi = p->i;

		int *pj = (int *)(pBuf + HeadLength + 8 + 8);
		*pj = p->j;

		return pBuf;
	}

	virtual ~CLMsg2ForCLNonThreadForMsgLoop_Serializer()
	{
		g_for_msg2_se++;
	}
};

class CLSTLQueue_CLNonThreadForMsgLoop : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		EXPECT_EQ((long)pContext, 3);

		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLSTLQueue_CLNonThreadForMsgLoop::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLSTLQueue_CLNonThreadForMsgLoop::On_2));

		CLMessage *p = new CLMsg1ForCLNonThreadForMsgLoop;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p).IsSuccess());

		p = new CLMsg2ForCLNonThreadForMsgLoop;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLNonThreadForMsgLoop *p = dynamic_cast<CLMsg1ForCLNonThreadForMsgLoop*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on1++;

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLNonThreadForMsgLoop *p = dynamic_cast<CLMsg2ForCLNonThreadForMsgLoop*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on2++;

		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

TEST(CLNonThreadForMsgLoop, STL_QUEUE)
{
	CLNonThreadForMsgLoop thread(new CLSTLQueue_CLNonThreadForMsgLoop, test_pipe_name, EXECUTIVE_IN_PROCESS_USE_STL_QUEUE);
	EXPECT_TRUE(thread.Run((void *)3).IsSuccess());

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, 1);
	EXPECT_EQ(g_for_msg1, 1);
	EXPECT_EQ(g_for_msg2, 1);
}

TEST(CLNonThreadForMsgLoop, PRIVATE_PIPE_QUEUE)
{
	g_for_on1 = 0;
	g_for_on2 = 0;
	g_for_msg1 = 0;
	g_for_msg2 = 0;

	CLNonThreadForMsgLoop thread(new CLSTLQueue_CLNonThreadForMsgLoop, test_pipe_name, EXECUTIVE_IN_PROCESS_USE_PIPE_QUEUE);
	EXPECT_TRUE(thread.Run((void *)3).IsSuccess());

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, 1);
	EXPECT_EQ(g_for_msg1, 1);
	EXPECT_EQ(g_for_msg2, 1);
}

class CLPipeQueue_CLNonThreadForMsgLoop : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		EXPECT_EQ((long)pContext, 3);

		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLPipeQueue_CLNonThreadForMsgLoop::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLPipeQueue_CLNonThreadForMsgLoop::On_2));

		CLSharedExecutiveCommunicationByNamedPipe *queue = new CLSharedExecutiveCommunicationByNamedPipe(test_pipe_name);
		EXPECT_TRUE(queue->RegisterSerializer(1, new CLMsg1ForCLNonThreadForMsgLoop_Serializer).IsSuccess());
		EXPECT_TRUE(queue->RegisterSerializer(2, new CLMsg2ForCLNonThreadForMsgLoop_Serializer).IsSuccess());

		EXPECT_TRUE(CLExecutiveNameServer::GetInstance()->Register(test_pipe_name, queue).IsSuccess());

		CLMessage *p = new CLMsg1ForCLNonThreadForMsgLoop;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p).IsSuccess());

		p = new CLMsg2ForCLNonThreadForMsgLoop;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p).IsSuccess());

		EXPECT_TRUE(CLExecutiveNameServer::GetInstance()->ReleaseCommunicationPtr(test_pipe_name).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLNonThreadForMsgLoop *p = dynamic_cast<CLMsg1ForCLNonThreadForMsgLoop*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on1++;

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLNonThreadForMsgLoop *p = dynamic_cast<CLMsg2ForCLNonThreadForMsgLoop*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on2++;

		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

TEST(CLNonThreadForMsgLoop, Shared_PIPE_QUEUE)
{
	g_for_on1 = 0;
	g_for_on2 = 0;
	g_for_msg1 = 0;
	g_for_msg2 = 0;

	{
		CLNonThreadForMsgLoop thread(new CLPipeQueue_CLNonThreadForMsgLoop, test_pipe_name, EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE);
		EXPECT_TRUE(thread.RegisterDeserializer(1, new CLMsg1ForCLNonThreadForMsgLoop_Deserializer).IsSuccess());
		EXPECT_TRUE(thread.RegisterDeserializer(2, new CLMsg2ForCLNonThreadForMsgLoop_Deserializer).IsSuccess());
		EXPECT_TRUE(thread.Run((void *)3).IsSuccess());
	}

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, 1);
	EXPECT_EQ(g_for_msg1, 2);
	EXPECT_EQ(g_for_msg2, 2);
	EXPECT_EQ(g_for_msg1_dese, 1);
	EXPECT_EQ(g_for_msg2_dese, 1);
	EXPECT_EQ(g_for_msg1_se, 1);
	EXPECT_EQ(g_for_msg2_se, 1);
}

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
}