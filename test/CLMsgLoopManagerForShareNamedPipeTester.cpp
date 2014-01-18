#include <gtest/gtest.h>
#include "LibExecutive.h"

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

		*ppMsg = new CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest();

		return CLStatus(0, 0);
	}

	virtual ~CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest_Deserializer()
	{
	}
};

static bool g1 = false;
static bool g2 = false;

class CLObserverTesterForCLMsgLoopManagerForShareNamedPipe : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLObserverTesterForCLMsgLoopManagerForShareNamedPipe::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLObserverTesterForCLMsgLoopManagerForShareNamedPipe::On_2));

		for(int i = 0; i < 10; i++)
		{
			EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("CLMsgLoopManagerForShareNamedPipeTester", new CLMsg1ForCLMsgLoopManagerForShareNamedPipeTest, true).IsSuccess());

			if(i == 5)
			{
				EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("CLMsgLoopManagerForShareNamedPipeTester", new CLMsg2ForCLMsgLoopManagerForShareNamedPipeTest, true).IsSuccess());
			}
		}

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		EXPECT_EQ(pm->m_clMsgID, 1);
		g1 = true;
		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		EXPECT_EQ(pm->m_clMsgID, 2);
		g2 = true;
		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

TEST(CLMsgLoopManagerForShareNamedPipe, Normal)
{
	CLLogger::WriteLogMsg("CLMsgLoopManagerForShareNamedPipe Test", 0);

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

	EXPECT_TRUE(g1);
	EXPECT_TRUE(g2);
}