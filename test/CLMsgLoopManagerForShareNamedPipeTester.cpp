#include <gtest/gtest.h>
#include <unistd.h>
#include "LibExecutive.h"

static unsigned int gmsg1 = 0;
static unsigned int gmsg2 = 0;

class CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest : public CLMessage
{
public:
	CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest() : CLMessage(1)
	{
		i = 2;
		j = 3;
	}

	virtual ~CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest()
	{
		gmsg1++;
	}

	int i;
	int j;
};

class CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest : public CLMessage
{
public:
	CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest() : CLMessage(2)
	{
	}

	virtual ~CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest()
	{
		gmsg2++;
	}
};

class CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest_Serializer : public CLMessageSerializer
{
public:
	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVectors *pIOVectors)
	{
		CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest *pm = dynamic_cast<CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest *>(pMsg);
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

	virtual ~CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest_Serializer()
	{
	}
};

class CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest_Serializer : public CLMessageSerializer
{
public:
	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVectors *pIOVectors)
	{
		CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest *pm = dynamic_cast<CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest *>(pMsg);
		EXPECT_TRUE(pm != 0);

		char *p = new char [12];

		int *pHead = (int *)p;
		*pHead = 8;

		long *pid = (long *)(p + 4);
		*pid = pm->m_clMsgID;

		EXPECT_TRUE(pIOVectors->PushBack(p, 12, true).IsSuccess());

		return CLStatus(0, 0);
	}

	virtual ~CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest_Serializer()
	{
	}
};

class CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLStatus Deserialize(CLIOVectors& IOVectors, CLMessage **ppMsg, CLBufferManager& BufferManager, void *context)
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

		*ppMsg = new CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest();

		return CLStatus(0, 0);
	}

	virtual ~CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest_Deserializer()
	{
	}
};

class CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLStatus Deserialize(CLIOVectors& IOVectors, CLMessage **ppMsg, CLBufferManager& BufferManager, void *context)
	{
		CLIteratorForIOVectors iter;
		IOVectors.GetIterator(0, iter);

		int len = 0;
		EXPECT_TRUE(IOVectors.ReadBlock(iter, (char *)(&len), 4).IsSuccess());
		EXPECT_EQ(len, 8);

		long id = 0;
		EXPECT_TRUE(IOVectors.ReadBlock(iter, (char *)(&id), 8).IsSuccess());
		EXPECT_EQ(id, 2);

		*ppMsg = new CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest();

		return CLStatus(0, 0);
	}

	virtual ~CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest_Deserializer()
	{
	}
};

static unsigned int g1 = 0;
static unsigned int g2 = 0;

class CLObserverTesterForCLMsgLoopManagerForShareNamedPipe : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLObserverTesterForCLMsgLoopManagerForShareNamedPipe::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLObserverTesterForCLMsgLoopManagerForShareNamedPipe::On_2));

		for(int i = 0; i < 300; i++)
		{
			EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("CLMsgLoopManagerForShareNamedPipeTester", new CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest, true).IsSuccess());

			if(i == 299)
			{
				EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("CLMsgLoopManagerForShareNamedPipeTester", new CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest, true).IsSuccess());
			}
		}

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		EXPECT_EQ(pm->m_clMsgID, 1);
		g1++;
		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		EXPECT_EQ(pm->m_clMsgID, 2);
		g2++;
		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

TEST(CLMsgLoopManagerForShareNamedPipe, Normal)
{
	CLLogger::WriteLogMsg("CLMsgLoopManagerForShareNamedPipe Test", 0);

	gmsg1 = 0;
	gmsg2 = 0;
	g1 = 0;
	g2 = 0;

	CLMultiMsgDeserializer *pd = new CLMultiMsgDeserializer();
	EXPECT_TRUE(pd->RegisterDeserializer(1, new CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest_Deserializer).IsSuccess());
	EXPECT_TRUE(pd->RegisterDeserializer(2, new CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest_Deserializer).IsSuccess());

	CLMultiMsgSerializer *ps = new CLMultiMsgSerializer();
	EXPECT_TRUE(ps->RegisterSerializer(1, new CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest_Serializer).IsSuccess());
	EXPECT_TRUE(ps->RegisterSerializer(2, new CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest_Serializer).IsSuccess());

	CLMessageLoopManager *pM = new CLMsgLoopManagerForShareNamedPipe(new CLObserverTesterForCLMsgLoopManagerForShareNamedPipe, "CLMsgLoopManagerForShareNamedPipeTester", 0, new CLProtocolDecapsulatorByDefaultMsgFormat(), ps, pd);

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	EXPECT_TRUE(pM->EnterMessageLoop(&s).IsSuccess());

	delete pM;

	EXPECT_EQ(gmsg1, 600);
	EXPECT_EQ(gmsg2, 2);

	EXPECT_EQ(g1, 300);
	EXPECT_EQ(g2, 1);
}

void *TestThread1ForCLMsgLoopManagerForSharedNamedPipe(void *)
{
	sleep(5);

	CLMultiMsgDeserializer *pd = new CLMultiMsgDeserializer();
	EXPECT_TRUE(pd->RegisterDeserializer(1, new CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest_Deserializer).IsSuccess());
	EXPECT_TRUE(pd->RegisterDeserializer(2, new CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest_Deserializer).IsSuccess());

	CLMultiMsgSerializer *ps = new CLMultiMsgSerializer();
	EXPECT_TRUE(ps->RegisterSerializer(1, new CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest_Serializer).IsSuccess());
	EXPECT_TRUE(ps->RegisterSerializer(2, new CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest_Serializer).IsSuccess());

	CLMessageLoopManager *pM = new CLMsgLoopManagerForShareNamedPipe(new CLObserverTesterForCLMsgLoopManagerForShareNamedPipe, "CLMsgLoopManagerForShareNamedPipeTester", 0, new CLProtocolDecapsulatorByDefaultMsgFormat(), ps, pd);

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	CLLogger::WriteLogMsg("3 The Following bug is produced on purpose 3", 0);
	EXPECT_FALSE(pM->EnterMessageLoop(&s).IsSuccess());

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	EXPECT_FALSE(pM->EnterMessageLoop(&s).IsSuccess());

	delete pM;

	EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("CLMsgLoopManagerForShareNamedPipeTester", new CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest, true).IsSuccess());
	EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("CLMsgLoopManagerForShareNamedPipeTester", new CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest, true).IsSuccess());

	return 0;
}

class CLObserverTester1ForCLMsgLoopManagerForShareNamedPipe : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLObserverTesterForCLMsgLoopManagerForShareNamedPipe::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLObserverTesterForCLMsgLoopManagerForShareNamedPipe::On_2));

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		EXPECT_EQ(pm->m_clMsgID, 1);
		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		EXPECT_EQ(pm->m_clMsgID, 2);
		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

TEST(CLMsgLoopManagerForShareNamedPipe, MultiRegister)
{
	CLMultiMsgDeserializer *pd = new CLMultiMsgDeserializer();
	EXPECT_TRUE(pd->RegisterDeserializer(1, new CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest_Deserializer).IsSuccess());
	EXPECT_TRUE(pd->RegisterDeserializer(2, new CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest_Deserializer).IsSuccess());

	CLMultiMsgSerializer *ps = new CLMultiMsgSerializer();
	EXPECT_TRUE(ps->RegisterSerializer(1, new CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest_Serializer).IsSuccess());
	EXPECT_TRUE(ps->RegisterSerializer(2, new CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest_Serializer).IsSuccess());

	CLMessageLoopManager *pM = new CLMsgLoopManagerForShareNamedPipe(new CLObserverTester1ForCLMsgLoopManagerForShareNamedPipe, "CLMsgLoopManagerForShareNamedPipeTester", 0, new CLProtocolDecapsulatorByDefaultMsgFormat(), ps, pd);

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	pthread_t tid;
	pthread_create(&tid, 0, TestThread1ForCLMsgLoopManagerForSharedNamedPipe, 0);

	EXPECT_TRUE(pM->EnterMessageLoop(&s).IsSuccess());

	delete pM;

	pthread_join(tid, 0);
}

class CLObserverTester1ForCLMsgLoopManagerForSharedNamedPipe : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		for(int i = 0; i < 300; i++)
		{
			EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("CLMsgLoopManagerForShareNamedPipeTester", new CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest, true).IsSuccess());

			if(i == 299)
			{
				EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("CLMsgLoopManagerForShareNamedPipeTester", new CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest, true).IsSuccess());
			}
		}

		return CLStatus(-1, NORMAL_ERROR);
	}
};

TEST(CLMsgLoopManagerForShareNamedPipe, ObserverInitializeFailure)
{
	CLMultiMsgDeserializer *pd = new CLMultiMsgDeserializer();
	EXPECT_TRUE(pd->RegisterDeserializer(1, new CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest_Deserializer).IsSuccess());
	EXPECT_TRUE(pd->RegisterDeserializer(2, new CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest_Deserializer).IsSuccess());

	CLMultiMsgSerializer *ps = new CLMultiMsgSerializer();
	EXPECT_TRUE(ps->RegisterSerializer(1, new CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest_Serializer).IsSuccess());
	EXPECT_TRUE(ps->RegisterSerializer(2, new CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest_Serializer).IsSuccess());

	CLMessageLoopManager *pM = new CLMsgLoopManagerForShareNamedPipe(new CLObserverTester1ForCLMsgLoopManagerForSharedNamedPipe, "CLMsgLoopManagerForShareNamedPipeTester", 0, new CLProtocolDecapsulatorByDefaultMsgFormat(), ps, pd);

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	EXPECT_FALSE((pM->EnterMessageLoop(&s)).IsSuccess());

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	EXPECT_FALSE((pM->EnterMessageLoop(&s)).IsSuccess());

	delete pM;
}