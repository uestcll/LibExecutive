#include <gtest/gtest.h>
#include <unistd.h>
#include "LibExecutive.h"

static unsigned int gmsg1 = 0;
static unsigned int gmsg2 = 0;
static unsigned int gmsg3 = 0;

class CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest : public CLMessage
{
public:
	CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest() : CLMessage(1)
	{
		i = 2;
		j = 3;
	}

	virtual ~CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest()
	{
		gmsg1++;
	}

	int i;
	int j;
};

class CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest : public CLMessage
{
public:
	CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest() : CLMessage(2)
	{
	}

	virtual ~CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest()
	{
		gmsg2++;
	}
};

class CLMsg3ForCLMsgLoopManagerForIOMultiplexingTest : public CLMessage
{
public:
	CLMsg3ForCLMsgLoopManagerForIOMultiplexingTest() : CLMessage(3)
	{
		i = 3;
	}

	virtual ~CLMsg3ForCLMsgLoopManagerForIOMultiplexingTest()
	{
		gmsg3++;
	}

	int i;
};

class CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest_Serializer : public CLMessageSerializer
{
public:
	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVectors *pIOVectors)
	{
		CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest *pm = dynamic_cast<CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest *>(pMsg);
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

	virtual ~CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest_Serializer()
	{
	}
};

class CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest_Serializer : public CLMessageSerializer
{
public:
	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVectors *pIOVectors)
	{
		CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest *pm = dynamic_cast<CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest *>(pMsg);
		EXPECT_TRUE(pm != 0);

		char *p = new char [12];

		int *pHead = (int *)p;
		*pHead = 8;

		long *pid = (long *)(p + 4);
		*pid = pm->m_clMsgID;

		EXPECT_TRUE(pIOVectors->PushBack(p, 12, true).IsSuccess());

		return CLStatus(0, 0);
	}

	virtual ~CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest_Serializer()
	{
	}
};

class CLMsg3ForCLMsgLoopManagerForIOMultiplexingTest_Serializer : public CLMessageSerializer
{
public:
	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVectors *pIOVectors)
	{
		CLMsg3ForCLMsgLoopManagerForIOMultiplexingTest *pm = dynamic_cast<CLMsg3ForCLMsgLoopManagerForIOMultiplexingTest *>(pMsg);
		EXPECT_TRUE(pm != 0);

		char *p = new char [16];

		int *pHead = (int *)p;
		*pHead = 12;

		long *pid = (long *)(p + 4);
		*pid = pm->m_clMsgID;

		int *pi = (int *)(p + 12);
		*pi = pm->i;
		
		EXPECT_TRUE(pIOVectors->PushBack(p, 16, true).IsSuccess());

		return CLStatus(0, 0);
	}

	virtual ~CLMsg3ForCLMsgLoopManagerForIOMultiplexingTest_Serializer()
	{
	}
};

class CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest_Deserializer : public CLMessageDeserializer
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

		*ppMsg = new CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest();

		return CLStatus(0, 0);
	}

	virtual ~CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest_Deserializer()
	{
	}
};

class CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest_Deserializer : public CLMessageDeserializer
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

		*ppMsg = new CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest();

		return CLStatus(0, 0);
	}

	virtual ~CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest_Deserializer()
	{
	}
};

class CLMsg3ForCLMsgLoopManagerForIOMultiplexingTest_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLStatus Deserialize(CLIOVectors& IOVectors, CLMessage **ppMsg, CLBufferManager& BufferManager, void *context)
	{
		CLIteratorForIOVectors iter;
		IOVectors.GetIterator(0, iter);

		int len = 0;
		EXPECT_TRUE(IOVectors.ReadBlock(iter, (char *)(&len), 4).IsSuccess());
		EXPECT_EQ(len, 12);

		long id = 0;
		EXPECT_TRUE(IOVectors.ReadBlock(iter, (char *)(&id), 8).IsSuccess());
		EXPECT_EQ(id, 3);

		int x = 0;
		EXPECT_TRUE(IOVectors.ReadBlock(iter, (char *)(&x), 4).IsSuccess());
		EXPECT_EQ(x, 3);

		*ppMsg = new CLMsg3ForCLMsgLoopManagerForIOMultiplexingTest();

		return CLStatus(0, 0);
	}

	virtual ~CLMsg3ForCLMsgLoopManagerForIOMultiplexingTest_Deserializer()
	{
	}
};

static unsigned int g1 = 0;
static unsigned int g2 = 0;
static unsigned int g3 = 0;

static int fd1 = 0;
static int fd2 = 0;

static CLUuid uu1;
static CLUuid uu2;

class CLObserverTesterForCLMsgLoopManagerForIOMultiplexing : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLObserverTesterForCLMsgLoopManagerForIOMultiplexing::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLObserverTesterForCLMsgLoopManagerForIOMultiplexing::On_2));
		pMessageLoop->Register(3, (CallBackForMessageLoop)(&CLObserverTesterForCLMsgLoopManagerForIOMultiplexing::On_3));

		m_pML = dynamic_cast<CLMsgLoopManagerForIOMultiplexing *>(pMessageLoop);
		EXPECT_TRUE(m_pML);

		for(int i = 0; i < 200; i++)
		{
			if(i % 2)
			{
				EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("PrivateNamedPipe_For_CLMsgLoopManagerForIOMultiplexing_Test", new CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest, false).IsSuccess());
			}
			else
			{
				EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("SharedNamedPipe_For_CLMsgLoopManagerForIOMultiplexing_Test", new CLMsg3ForCLMsgLoopManagerForIOMultiplexingTest, true).IsSuccess());
			}
		}

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm, CLUuid u1)
	{
		EXPECT_EQ(pm->m_clMsgID, 1);
		
		CLUuidComparer compare1;
		EXPECT_TRUE(compare1(u1, uu1) == 0);

		g1++;
		if(g1 == 100)
		{
			EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("SharedNamedPipe_For_CLMsgLoopManagerForIOMultiplexing_Test", new CLMsg3ForCLMsgLoopManagerForIOMultiplexingTest, true).IsSuccess());	
			EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("PrivateNamedPipe_For_CLMsgLoopManagerForIOMultiplexing_Test", new CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest, false).IsSuccess());
		}

		return CLStatus(0, 0);
	}

	CLStatus On_3(CLMessage *pm, CLUuid u1)
	{
		EXPECT_EQ(pm->m_clMsgID, 3);
		
		CLUuidComparer compare1;
		EXPECT_TRUE(compare1(u1, uu2) == 0);

		g3++;

		if(g3 == 100)
		{
			EXPECT_TRUE(m_pML->UnRegisterReadEvent(fd2).IsSuccess());
		}

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm, CLUuid u1)
	{
		EXPECT_EQ(pm->m_clMsgID, 2);

		CLUuidComparer compare1;
		EXPECT_TRUE(compare1(u1, uu1) == 0);

		g2++;
		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}

	CLMsgLoopManagerForIOMultiplexing *m_pML;
};

TEST(CLMsgLoopManagerForIOMultiplexing, SingleThreadNormalForPipe)
{
	CLLogger::WriteLogMsg("CLMsgLoopManagerForIOMultiplexing Test", 0);

	gmsg1 = 0;
	gmsg2 = 0;
	g1 = 0;
	g2 = 0;

	CLMultiMsgDeserializer *pd = new CLMultiMsgDeserializer();
	EXPECT_TRUE(pd->RegisterDeserializer(1, new CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest_Deserializer).IsSuccess());
	EXPECT_TRUE(pd->RegisterDeserializer(2, new CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest_Deserializer).IsSuccess());
	EXPECT_TRUE(pd->RegisterDeserializer(3, new CLMsg3ForCLMsgLoopManagerForIOMultiplexingTest_Deserializer).IsSuccess());

	CLMultiMsgSerializer *ps = new CLMultiMsgSerializer();
	EXPECT_TRUE(ps->RegisterSerializer(1, new CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest_Serializer).IsSuccess());
	EXPECT_TRUE(ps->RegisterSerializer(2, new CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest_Serializer).IsSuccess());
	EXPECT_TRUE(ps->RegisterSerializer(3, new CLMsg3ForCLMsgLoopManagerForIOMultiplexingTest_Serializer).IsSuccess());

	CLMsgLoopManagerForIOMultiplexing *pM = new CLMsgLoopManagerForIOMultiplexing(new CLObserverTesterForCLMsgLoopManagerForIOMultiplexing, "CLMsgLoopManagerForIOMultiplexingTester", false);

	//For private pipe register
	const char *privatePipeStrPath = "/tmp/PrivateNamedPipe_For_CLMsgLoopManagerForIOMultiplexing_Test";

	CLDataReceiver *pPrivatePipeDataReceiver = new CLDataReceiverByNamedPipe(privatePipeStrPath);
	uu1 = pPrivatePipeDataReceiver->GetUuid();
	fd1 = pPrivatePipeDataReceiver->GetFd();
	CLMessageReceiver *pMsgReceiverForPrivatePipe = new CLMessageReceiver(&uu1, new CLBufferManager(), pPrivatePipeDataReceiver, new CLPointerToMsgDeserializer(), new CLProtocolDecapsulatorBySplitPointer());

	EXPECT_TRUE(pM->RegisterReadEvent(fd1, pMsgReceiverForPrivatePipe).IsSuccess());
	
	CLLogger::WriteLogMsg("The Following bug is produced on purpose ", 0);
	CLDataReceiver *pErrorDataReceiver = new CLDataReceiverByNamedPipe("/tmp/ErrorNamedPipe_For_CLMsgLoopManagerForIOMultiplexing_Test");
	CLUuid uuu = pPrivatePipeDataReceiver->GetUuid();
	CLMessageReceiver *pErrorMsgReceiver = new CLMessageReceiver(&uuu, new CLBufferManager(), pErrorDataReceiver, new CLPointerToMsgDeserializer(), new CLProtocolDecapsulatorBySplitPointer());
	CLStatus ss = pM->RegisterReadEvent(fd1, pErrorMsgReceiver);
	EXPECT_FALSE(ss.IsSuccess());
	EXPECT_EQ(ss.m_clErrorCode, NORMAL_ERROR);
	CLLogger::WriteLogMsg("The Following bug is produced on purpose end", 0);

	CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
	EXPECT_TRUE(pNameServer);

	CLMessagePoster *pPrivatePipeMsgPoster = new CLMessagePoster(new CLMsgToPointerSerializer(), 0, new CLDataPostChannelByNamedPipeMaintainer(privatePipeStrPath), 0);
	EXPECT_TRUE((pPrivatePipeMsgPoster->Initialize(new CLInitialDataPostChannelNotifier(), 0)).IsSuccess());
	EXPECT_TRUE(pNameServer->Register("PrivateNamedPipe_For_CLMsgLoopManagerForIOMultiplexing_Test", uu1, pPrivatePipeMsgPoster, pMsgReceiverForPrivatePipe).IsSuccess());

	//For shared pipe register
	const char *pSharedPipeStrPath = "/tmp/SharedNamedPipe_For_CLMsgLoopManagerForIOMultiplexing_Test";

	CLDataReceiver *pSharedPipeDataReceiver = new CLDataReceiverByNamedPipe(pSharedPipeStrPath);
	uu2 = pSharedPipeDataReceiver->GetUuid();
	fd2 = pSharedPipeDataReceiver->GetFd();
	CLMessageReceiver *pMsgReceiverForSharedPipe = new CLMessageReceiver(&uu2, new CLBufferManager(), pSharedPipeDataReceiver, pd, new CLProtocolDecapsulatorByDefaultMsgFormat());

	EXPECT_TRUE(pM->RegisterReadEvent(fd2, pMsgReceiverForSharedPipe).IsSuccess());

	CLMessagePoster *pSharedPipeMsgPoster = new CLMessagePoster(ps, 0, new CLDataPostChannelByNamedPipeMaintainer(pSharedPipeStrPath), 0);
	EXPECT_TRUE((pSharedPipeMsgPoster->Initialize(new CLInitialDataPostChannelNotifier(), 0)).IsSuccess());
	EXPECT_TRUE(pNameServer->Register("SharedNamedPipe_For_CLMsgLoopManagerForIOMultiplexing_Test", uu2, pSharedPipeMsgPoster).IsSuccess());


	//For listen socket fd register
	//For conn socket fd register



	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	EXPECT_TRUE(pM->EnterMessageLoop(&s).IsSuccess());

	EXPECT_TRUE(pNameServer->ReleaseCommunicationPtr("PrivateNamedPipe_For_CLMsgLoopManagerForIOMultiplexing_Test").IsSuccess());
	EXPECT_TRUE(pNameServer->ReleaseCommunicationPtr("SharedNamedPipe_For_CLMsgLoopManagerForIOMultiplexing_Test").IsSuccess());

	delete pM;

	EXPECT_EQ(gmsg1, 100);
	EXPECT_EQ(gmsg3, 201);
	EXPECT_EQ(gmsg2, 1);

	EXPECT_EQ(g1, 100);
	EXPECT_EQ(g2, 1);
	EXPECT_EQ(g3, 100);
}

// void *TestThread1ForCLMsgLoopManagerForSharedNamedPipe(void *)
// {
// 	sleep(5);

// 	CLMultiMsgDeserializer *pd = new CLMultiMsgDeserializer();
// 	EXPECT_TRUE(pd->RegisterDeserializer(1, new CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest_Deserializer).IsSuccess());
// 	EXPECT_TRUE(pd->RegisterDeserializer(2, new CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest_Deserializer).IsSuccess());

// 	CLMultiMsgSerializer *ps = new CLMultiMsgSerializer();
// 	EXPECT_TRUE(ps->RegisterSerializer(1, new CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest_Serializer).IsSuccess());
// 	EXPECT_TRUE(ps->RegisterSerializer(2, new CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest_Serializer).IsSuccess());

// 	CLMessageLoopManager *pM = new CLMsgLoopManagerForIOMultiplexing(new CLObserverTesterForCLMsgLoopManagerForIOMultiplexing, "CLMsgLoopManagerForIOMultiplexingTester", 0, new CLProtocolDecapsulatorByDefaultMsgFormat(), ps, pd);

// 	SLExecutiveInitialParameter s;
// 	s.pContext = 0;
// 	CLThreadInitialFinishedNotifier notifier(0);
// 	s.pNotifier = &notifier;

// 	CLLogger::WriteLogMsg("3 The Following bug is produced on purpose 3", 0);
// 	EXPECT_FALSE(pM->EnterMessageLoop(&s).IsSuccess());

// 	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
// 	EXPECT_FALSE(pM->EnterMessageLoop(&s).IsSuccess());

// 	delete pM;

// 	EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("CLMsgLoopManagerForIOMultiplexingTester", new CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest, true).IsSuccess());
// 	EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("CLMsgLoopManagerForIOMultiplexingTester", new CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest, true).IsSuccess());

// 	return 0;
// }

// class CLObserverTester1ForCLMsgLoopManagerForIOMultiplexing : public CLMessageObserver
// {
// public:
// 	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
// 	{
// 		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLObserverTesterForCLMsgLoopManagerForIOMultiplexing::On_1));
// 		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLObserverTesterForCLMsgLoopManagerForIOMultiplexing::On_2));

// 		return CLStatus(0, 0);
// 	}

// 	CLStatus On_1(CLMessage *pm, CLUuid u1)
// 	{
// 		EXPECT_EQ(pm->m_clMsgID, 1);
// 		return CLStatus(0, 0);
// 	}

// 	CLStatus On_2(CLMessage *pm, CLUuid u1)
// 	{
// 		EXPECT_EQ(pm->m_clMsgID, 2);
// 		return CLStatus(QUIT_MESSAGE_LOOP, 0);
// 	}
// };

// TEST(CLMsgLoopManagerForIOMultiplexing, MultiRegister)
// {
// 	CLMultiMsgDeserializer *pd = new CLMultiMsgDeserializer();
// 	EXPECT_TRUE(pd->RegisterDeserializer(1, new CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest_Deserializer).IsSuccess());
// 	EXPECT_TRUE(pd->RegisterDeserializer(2, new CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest_Deserializer).IsSuccess());

// 	CLMultiMsgSerializer *ps = new CLMultiMsgSerializer();
// 	EXPECT_TRUE(ps->RegisterSerializer(1, new CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest_Serializer).IsSuccess());
// 	EXPECT_TRUE(ps->RegisterSerializer(2, new CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest_Serializer).IsSuccess());

// 	CLMessageLoopManager *pM = new CLMsgLoopManagerForIOMultiplexing(new CLObserverTester1ForCLMsgLoopManagerForIOMultiplexing, "CLMsgLoopManagerForIOMultiplexingTester", 0, new CLProtocolDecapsulatorByDefaultMsgFormat(), ps, pd);

// 	SLExecutiveInitialParameter s;
// 	s.pContext = 0;
// 	CLThreadInitialFinishedNotifier notifier(0);
// 	s.pNotifier = &notifier;

// 	pthread_t tid;
// 	pthread_create(&tid, 0, TestThread1ForCLMsgLoopManagerForSharedNamedPipe, 0);

// 	EXPECT_TRUE(pM->EnterMessageLoop(&s).IsSuccess());

// 	delete pM;

// 	pthread_join(tid, 0);
// }

// class CLObserverTester1ForCLMsgLoopManagerForSharedNamedPipe : public CLMessageObserver
// {
// public:
// 	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
// 	{
// 		for(int i = 0; i < 300; i++)
// 		{
// 			EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("CLMsgLoopManagerForIOMultiplexingTester", new CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest, true).IsSuccess());

// 			if(i == 299)
// 			{
// 				EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("CLMsgLoopManagerForIOMultiplexingTester", new CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest, true).IsSuccess());
// 			}
// 		}

// 		return CLStatus(-1, NORMAL_ERROR);
// 	}
// };

// TEST(CLMsgLoopManagerForIOMultiplexing, ObserverInitializeFailure)
// {
// 	CLMultiMsgDeserializer *pd = new CLMultiMsgDeserializer();
// 	EXPECT_TRUE(pd->RegisterDeserializer(1, new CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest_Deserializer).IsSuccess());
// 	EXPECT_TRUE(pd->RegisterDeserializer(2, new CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest_Deserializer).IsSuccess());

// 	CLMultiMsgSerializer *ps = new CLMultiMsgSerializer();
// 	EXPECT_TRUE(ps->RegisterSerializer(1, new CLMsg1ForCLMsgLoopManagerForIOMultiplexingTest_Serializer).IsSuccess());
// 	EXPECT_TRUE(ps->RegisterSerializer(2, new CLMsg2ForCLMsgLoopManagerForIOMultiplexingTest_Serializer).IsSuccess());

// 	CLMessageLoopManager *pM = new CLMsgLoopManagerForIOMultiplexing(new CLObserverTester1ForCLMsgLoopManagerForSharedNamedPipe, "CLMsgLoopManagerForIOMultiplexingTester", 0, new CLProtocolDecapsulatorByDefaultMsgFormat(), ps, pd);

// 	SLExecutiveInitialParameter s;
// 	s.pContext = 0;
// 	CLThreadInitialFinishedNotifier notifier(0);
// 	s.pNotifier = &notifier;

// 	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
// 	EXPECT_FALSE((pM->EnterMessageLoop(&s)).IsSuccess());

// 	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
// 	EXPECT_FALSE((pM->EnterMessageLoop(&s)).IsSuccess());

// 	delete pM;
// }