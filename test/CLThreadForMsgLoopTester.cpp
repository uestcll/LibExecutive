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

static const char *test_pipe_name = "test_for_CLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg";

static int g_for_on1 = 0;
static int g_for_on2 = 0;
static int g_for_msg1 = 0;
static int g_for_msg2 = 0;
static int g_for_msg1_dese = 0;
static int g_for_msg2_dese = 0;
static int g_for_msg1_se = 0;
static int g_for_msg2_se = 0;

class CLMsg1ForCLThreadForMsgLoop : public CLMessage
{
public:
	CLMsg1ForCLThreadForMsgLoop() : CLMessage(1)
	{
		i = 2;
		j = 3;
	}

	virtual ~CLMsg1ForCLThreadForMsgLoop()
	{
		g_for_msg1++;
	}

	int i;
	int j;
};

class CLMsg2ForCLThreadForMsgLoop : public CLMessage
{
public:
	CLMsg2ForCLThreadForMsgLoop() : CLMessage(2)
	{
		i = 4;
		j = 6;
	}

	virtual ~CLMsg2ForCLThreadForMsgLoop()
	{
		g_for_msg2++;
	}

	long i;
	int j;
};

class CLMsg1ForCLThreadForMsgLoop_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		CLMsg1ForCLThreadForMsgLoop *p = new CLMsg1ForCLThreadForMsgLoop;
		p->i = *((int *)(pBuffer + sizeof(long)));
		p->j = *((int *)(pBuffer + sizeof(long) + sizeof(int)));
		return p;
	}

	virtual ~CLMsg1ForCLThreadForMsgLoop_Deserializer()
	{
		g_for_msg1_dese++;
	}
};

class CLMsg2ForCLThreadForMsgLoop_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		CLMsg2ForCLThreadForMsgLoop *p = new CLMsg2ForCLThreadForMsgLoop;
		p->i = *((long *)(pBuffer + sizeof(long)));
		p->j = *((int *)(pBuffer + sizeof(long) + sizeof(long)));
		return p;
	}

	virtual ~CLMsg2ForCLThreadForMsgLoop_Deserializer()
	{
		g_for_msg2_dese++;
	}
};

class CLMsg1ForCLThreadForMsgLoop_Serializer : public CLMessageSerializer
{
public:
	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength, unsigned int HeadLength)
	{
		CLMsg1ForCLThreadForMsgLoop *p = dynamic_cast<CLMsg1ForCLThreadForMsgLoop *>(pMsg);
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

	virtual ~CLMsg1ForCLThreadForMsgLoop_Serializer()
	{
		g_for_msg1_se++;
	}
};

class CLMsg2ForCLThreadForMsgLoop_Serializer : public CLMessageSerializer
{
public:
	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength, unsigned int HeadLength)
	{
		CLMsg2ForCLThreadForMsgLoop *p = dynamic_cast<CLMsg2ForCLThreadForMsgLoop *>(pMsg);
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

	virtual ~CLMsg2ForCLThreadForMsgLoop_Serializer()
	{
		g_for_msg2_se++;
	}
};

class CLSTLQueue_CLThreadForMsgLoop : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		EXPECT_EQ((long)pContext, 3);

		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLSTLQueue_CLThreadForMsgLoop::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLSTLQueue_CLThreadForMsgLoop::On_2));

		CLMessage *p = new CLMsg1ForCLThreadForMsgLoop;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p).IsSuccess());

		p = new CLMsg2ForCLThreadForMsgLoop;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLThreadForMsgLoop *p = dynamic_cast<CLMsg1ForCLThreadForMsgLoop*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on1++;

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLThreadForMsgLoop *p = dynamic_cast<CLMsg2ForCLThreadForMsgLoop*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on2++;

		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

TEST(CLThreadForMsgLoop, STL_QUEUE)
{
	{
		CLThreadForMsgLoop thread(new CLSTLQueue_CLThreadForMsgLoop, test_pipe_name, true, EXECUTIVE_IN_PROCESS_USE_STL_QUEUE);
		EXPECT_TRUE(thread.Run((void *)3).IsSuccess());
	}

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, 1);
	EXPECT_EQ(g_for_msg1, 1);
	EXPECT_EQ(g_for_msg2, 1);
}


TEST(CLThreadForMsgLoop, PRIVATE_PIPE_QUEUE)
{
	g_for_on1 = 0;
	g_for_on2 = 0;
	g_for_msg1 = 0;
	g_for_msg2 = 0;

	{
		CLThreadForMsgLoop thread(new CLSTLQueue_CLThreadForMsgLoop, test_pipe_name, true, EXECUTIVE_IN_PROCESS_USE_PIPE_QUEUE);
		EXPECT_TRUE(thread.Run((void *)3).IsSuccess());
	}

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, 1);
	EXPECT_EQ(g_for_msg1, 1);
	EXPECT_EQ(g_for_msg2, 1);
}

class CLPipeQueue_CLThreadForMsgLoop : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		EXPECT_EQ((long)pContext, 3);

		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLPipeQueue_CLThreadForMsgLoop::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLPipeQueue_CLThreadForMsgLoop::On_2));

		CLSharedExecutiveCommunicationByNamedPipe *queue = new CLSharedExecutiveCommunicationByNamedPipe(test_pipe_name);
		EXPECT_TRUE(queue->RegisterSerializer(1, new CLMsg1ForCLThreadForMsgLoop_Serializer).IsSuccess());
		EXPECT_TRUE(queue->RegisterSerializer(2, new CLMsg2ForCLThreadForMsgLoop_Serializer).IsSuccess());

		EXPECT_TRUE(CLExecutiveNameServer::GetInstance()->Register(test_pipe_name, queue).IsSuccess());

		CLMessage *p = new CLMsg1ForCLThreadForMsgLoop;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p).IsSuccess());

		p = new CLMsg2ForCLThreadForMsgLoop;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p).IsSuccess());

		EXPECT_TRUE(CLExecutiveNameServer::GetInstance()->ReleaseCommunicationPtr(test_pipe_name).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLThreadForMsgLoop *p = dynamic_cast<CLMsg1ForCLThreadForMsgLoop*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on1++;

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLThreadForMsgLoop *p = dynamic_cast<CLMsg2ForCLThreadForMsgLoop*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on2++;

		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

TEST(CLThreadForMsgLoop, Shared_PIPE_QUEUE)
{
	g_for_on1 = 0;
	g_for_on2 = 0;
	g_for_msg1 = 0;
	g_for_msg2 = 0;

	{
		CLThreadForMsgLoop thread(new CLPipeQueue_CLThreadForMsgLoop, test_pipe_name, true, EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE);
		EXPECT_TRUE(thread.RegisterDeserializer(1, new CLMsg1ForCLThreadForMsgLoop_Deserializer).IsSuccess());
		EXPECT_TRUE(thread.RegisterDeserializer(2, new CLMsg2ForCLThreadForMsgLoop_Deserializer).IsSuccess());
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