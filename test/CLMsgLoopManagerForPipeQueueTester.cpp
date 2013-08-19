#include <gtest/gtest.h>
#include "LibExecutive.h"

static const char *test_pipe_name = "test_for_CLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg";

static int g_for_on1 = 0;
static int g_for_on2 = 0;
static int g_for_msg1 = 0;
static int g_for_msg2 = 0;
static int g_for_msg1_dese = 0;
static int g_for_msg2_dese = 0;
static int g_for_msg1_se = 0;
static int g_for_msg2_se = 0;

class CLMsg1ForCLMsgLoopManagerForPipeQueue : public CLMessage
{
public:
	CLMsg1ForCLMsgLoopManagerForPipeQueue() : CLMessage(1)
	{
		i = 2;
		j = 3;
	}

	virtual ~CLMsg1ForCLMsgLoopManagerForPipeQueue()
	{
		g_for_msg1++;
	}

	int i;
	int j;
};

class CLMsg2ForCLMsgLoopManagerForPipeQueue : public CLMessage
{
public:
	CLMsg2ForCLMsgLoopManagerForPipeQueue() : CLMessage(2)
	{
		i = 4;
		j = 6;
	}

	virtual ~CLMsg2ForCLMsgLoopManagerForPipeQueue()
	{
		g_for_msg2++;
	}

	long i;
	int j;
};

class CLMsg1ForCLMsgLoopManagerForPipeQueue_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		CLMsg1ForCLMsgLoopManagerForPipeQueue *p = new CLMsg1ForCLMsgLoopManagerForPipeQueue;
		p->i = *((int *)(pBuffer + sizeof(long)));
		p->j = *((int *)(pBuffer + sizeof(long) + sizeof(int)));
		return p;
	}

	virtual ~CLMsg1ForCLMsgLoopManagerForPipeQueue_Deserializer()
	{
		g_for_msg1_dese++;
	}
};

class CLMsg2ForCLMsgLoopManagerForPipeQueue_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		CLMsg2ForCLMsgLoopManagerForPipeQueue *p = new CLMsg2ForCLMsgLoopManagerForPipeQueue;
		p->i = *((long *)(pBuffer + sizeof(long)));
		p->j = *((int *)(pBuffer + sizeof(long) + sizeof(long)));
		return p;
	}

	virtual ~CLMsg2ForCLMsgLoopManagerForPipeQueue_Deserializer()
	{
		g_for_msg2_dese++;
	}
};

class CLMsg1ForCLMsgLoopManagerForPipeQueue_Serializer : public CLMessageSerializer
{
public:
	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength, unsigned int HeadLength)
	{
		CLMsg1ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg1ForCLMsgLoopManagerForPipeQueue *>(pMsg);
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

	virtual ~CLMsg1ForCLMsgLoopManagerForPipeQueue_Serializer()
	{
		g_for_msg1_se++;
	}
};

class CLMsg2ForCLMsgLoopManagerForPipeQueue_Serializer : public CLMessageSerializer
{
public:
	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength, unsigned int HeadLength)
	{
		CLMsg2ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg2ForCLMsgLoopManagerForPipeQueue *>(pMsg);
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

	virtual ~CLMsg2ForCLMsgLoopManagerForPipeQueue_Serializer()
	{
		g_for_msg2_se++;
	}
};

class CLPrivateQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLPrivateQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLPrivateQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue::On_2));

		CLMessage *p = new CLMsg1ForCLMsgLoopManagerForPipeQueue;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p).IsSuccess());

		p = new CLMsg2ForCLMsgLoopManagerForPipeQueue;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg1ForCLMsgLoopManagerForPipeQueue*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on1++;

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg2ForCLMsgLoopManagerForPipeQueue*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on2++;

		CLMessage *p1 = new CLMsg2ForCLMsgLoopManagerForPipeQueue;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p1).IsSuccess());

		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

TEST(CLMsgLoopManagerForPipeQueue, PrivateQueueForSelfPostMsg)
{
	CLMessageLoopManager *pM = new CLMsgLoopManagerForPipeQueue(new CLPrivateQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue, test_pipe_name, PIPE_QUEUE_IN_PROCESS);

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	EXPECT_TRUE((pM->EnterMessageLoop(&s)).IsSuccess());

	delete pM;

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, 1);
	EXPECT_EQ(g_for_msg1, 1);
	EXPECT_EQ(g_for_msg2, 2);
}

class CLThreadForTestingCLMsgLoopManagerForPipeQueue_PrivateQueueForPostMsg : public CLExecutiveFunctionProvider
{
public:
	virtual CLStatus RunExecutiveFunction(void* pContext)
	{
		CLMessage *p = new CLMsg1ForCLMsgLoopManagerForPipeQueue;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p).IsSuccess());

		p = new CLMsg2ForCLMsgLoopManagerForPipeQueue;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p).IsSuccess());

		return CLStatus(0, 0);
	}
};

class CLPrivateQueueForPostMsg_CLMsgLoopManagerForPipeQueue : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLPrivateQueueForPostMsg_CLMsgLoopManagerForPipeQueue::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLPrivateQueueForPostMsg_CLMsgLoopManagerForPipeQueue::On_2));

		CLThread *p = new CLThread(new CLThreadForTestingCLMsgLoopManagerForPipeQueue_PrivateQueueForPostMsg, false);
		EXPECT_TRUE(p->Run().IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg1ForCLMsgLoopManagerForPipeQueue*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on1++;

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg2ForCLMsgLoopManagerForPipeQueue*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on2++;

		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

TEST(CLMsgLoopManagerForPipeQueue, PrivateQueueForfPostMsg)
{
	g_for_on1 = 0;
	g_for_on2 = 0;
	g_for_msg1 = 0;
	g_for_msg2 = 0;

	CLMessageLoopManager *pM = new CLMsgLoopManagerForPipeQueue(new CLPrivateQueueForPostMsg_CLMsgLoopManagerForPipeQueue, test_pipe_name, PIPE_QUEUE_IN_PROCESS);

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	EXPECT_TRUE((pM->EnterMessageLoop(&s)).IsSuccess());

	delete pM;

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, 1);
	EXPECT_EQ(g_for_msg1, 1);
	EXPECT_EQ(g_for_msg2, 1);
}

class CLSharedQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLSharedQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLSharedQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue::On_2));

		CLSharedExecutiveCommunicationByNamedPipe sender(test_pipe_name);
		EXPECT_TRUE(sender.RegisterSerializer(1, new CLMsg1ForCLMsgLoopManagerForPipeQueue_Serializer).IsSuccess());
		EXPECT_TRUE(sender.RegisterSerializer(2, new CLMsg2ForCLMsgLoopManagerForPipeQueue_Serializer).IsSuccess());

		sender.PostExecutiveMessage(new CLMsg1ForCLMsgLoopManagerForPipeQueue);
		sender.PostExecutiveMessage(new CLMsg2ForCLMsgLoopManagerForPipeQueue);

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg1ForCLMsgLoopManagerForPipeQueue*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on1++;

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg2ForCLMsgLoopManagerForPipeQueue*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on2++;

		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

TEST(CLMsgLoopManagerForPipeQueue, SharedQueueForSelfPostMsg)
{
	g_for_on1 = 0;
	g_for_on2 = 0;
	g_for_msg1 = 0;
	g_for_msg2 = 0;
	g_for_msg1_dese = 0;
	g_for_msg2_dese = 0;
	g_for_msg1_se = 0;
	g_for_msg2_se = 0;

	CLMsgLoopManagerForPipeQueue *pM = new CLMsgLoopManagerForPipeQueue(new CLSharedQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue, test_pipe_name, PIPE_QUEUE_BETWEEN_PROCESS);
	pM->RegisterDeserializer(1, new CLMsg1ForCLMsgLoopManagerForPipeQueue_Deserializer);
	pM->RegisterDeserializer(2, new CLMsg2ForCLMsgLoopManagerForPipeQueue_Deserializer);

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	EXPECT_TRUE((pM->EnterMessageLoop(&s)).IsSuccess());

	delete pM;

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, 1);
	EXPECT_EQ(g_for_msg1, 2);
	EXPECT_EQ(g_for_msg2, 2);
	EXPECT_EQ(g_for_msg1_dese, 1);
	EXPECT_EQ(g_for_msg2_dese, 1);
	EXPECT_EQ(g_for_msg1_se, 1);
	EXPECT_EQ(g_for_msg2_se, 1);
}

class CLSharedQueueForProcessPostMsg_CLMsgLoopManagerForPipeQueue : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLSharedQueueForProcessPostMsg_CLMsgLoopManagerForPipeQueue::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLSharedQueueForProcessPostMsg_CLMsgLoopManagerForPipeQueue::On_2));

		CLExecutive *process = new CLProcess(new CLProcessFunctionForExec, true);
		EXPECT_TRUE((process->Run((void *)"../test_for_exec/test_for_CLExecutiveCommunicationByNamedPipe/main")).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg1ForCLMsgLoopManagerForPipeQueue*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on1++;

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg2ForCLMsgLoopManagerForPipeQueue*>(pm);
		EXPECT_TRUE(p != 0);

		g_for_on2++;

		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

TEST(CLMsgLoopManagerForPipeQueue, SharedQueueForProcessPostMsg)
{
	g_for_on1 = 0;
	g_for_on2 = 0;
	g_for_msg1 = 0;
	g_for_msg2 = 0;
	g_for_msg1_dese = 0;
	g_for_msg2_dese = 0;
	g_for_msg1_se = 0;
	g_for_msg2_se = 0;

	CLMsgLoopManagerForPipeQueue *pM = new CLMsgLoopManagerForPipeQueue(new CLSharedQueueForProcessPostMsg_CLMsgLoopManagerForPipeQueue, test_pipe_name, PIPE_QUEUE_BETWEEN_PROCESS);
	pM->RegisterDeserializer(1, new CLMsg1ForCLMsgLoopManagerForPipeQueue_Deserializer);
	pM->RegisterDeserializer(2, new CLMsg2ForCLMsgLoopManagerForPipeQueue_Deserializer);

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	EXPECT_TRUE((pM->EnterMessageLoop(&s)).IsSuccess());

	delete pM;

	EXPECT_EQ(g_for_on1, 1);
	EXPECT_EQ(g_for_on2, 1);
	EXPECT_EQ(g_for_msg1, 1);
	EXPECT_EQ(g_for_msg2, 1);
	EXPECT_EQ(g_for_msg1_dese, 1);
	EXPECT_EQ(g_for_msg2_dese, 1);
	EXPECT_EQ(g_for_msg1_se, 0);
	EXPECT_EQ(g_for_msg2_se, 0);
}