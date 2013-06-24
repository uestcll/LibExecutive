#include <gtest/gtest.h>
#include "LibExecutive.h"

class CLMsg1ForPipe : public CLMessage
{
public:
	CLMsg1ForPipe() : CLMessage(1)
	{
		i = 2;
		j = 3;
	}

	int i;
	int j;
};

class CLMsg2ForPipe : public CLMessage
{
public:
	CLMsg2ForPipe() : CLMessage(2)
	{
		i = 4;
		j = 6;
	}

	long i;
	int j;
};

class CLMsg1Deserializer : public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		CLMsg1ForPipe *p = new CLMsg1ForPipe;
		p->i = *((int *)(pBuffer + sizeof(long)));
		p->j = *((int *)(pBuffer + sizeof(long) + sizeof(int)));
		return p;
	}
};

class CLMsg2Deserializer : public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		CLMsg2ForPipe *p = new CLMsg2ForPipe;
		p->i = *((long *)(pBuffer + sizeof(long)));
		p->j = *((int *)(pBuffer + sizeof(long) + sizeof(long)));
		return p;
	}
};

static int count;

class CLTestObserverForPipe : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		count = 0;

		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLTestObserverForPipe::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLTestObserverForPipe::On_2));

		CLExecutive *process = new CLProcess(new CLProcessFunctionForExec, false);
		EXPECT_TRUE((process->Run((void *)"../test_for_exec/test_for_CLMessageByNamedPipe/main")).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForPipe *p = dynamic_cast<CLMsg1ForPipe*>(pm);
		EXPECT_TRUE(p != 0);
		
		count++;

		if(count == 10)
			return CLStatus(QUIT_MESSAGE_LOOP, 0);

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForPipe *p = dynamic_cast<CLMsg2ForPipe*>(pm);
		EXPECT_TRUE(p != 0);
		
		count++;
		if(count == 10)
			return CLStatus(QUIT_MESSAGE_LOOP, 0);

		return CLStatus(0, 0);
	}
};

TEST(CLMessageQueueByNamedPipe, Normal)
{
	CLNonThreadForMsgLoop thread1(new CLTestObserverForPipe, "test_pipe", EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE);
	thread1.RegisterDeserializer(1, new CLMsg1Deserializer);
	thread1.RegisterDeserializer(2, new CLMsg2Deserializer);

	EXPECT_TRUE(thread1.Run(0).IsSuccess());

	EXPECT_EQ(count, 10);

	EXPECT_TRUE(true);

	sleep(2);
}

TEST(CLMessageQueueByNamedPipe, Normal_Thread)
{
	{
		CLThreadForMsgLoop thread1(new CLTestObserverForPipe, "test_pipe", true, EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE);
		thread1.RegisterDeserializer(1, new CLMsg1Deserializer);
		thread1.RegisterDeserializer(2, new CLMsg2Deserializer);

		EXPECT_TRUE(thread1.Run(0).IsSuccess());
	}

	EXPECT_EQ(count, 10);

	EXPECT_TRUE(true);
	
	sleep(2);
}

class CLMsg1Serializer : public CLMessageSerializer
{
public:
	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength, unsigned int HeadLength)
	{
		CLMsg1ForPipe *p = dynamic_cast<CLMsg1ForPipe *>(pMsg);
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
};

class CLMsg2Serializer : public CLMessageSerializer
{
public:
	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength, unsigned int HeadLength)
	{
		CLMsg2ForPipe *p = dynamic_cast<CLMsg2ForPipe *>(pMsg);
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
};

class CLTestObserver2ForPipe : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLTestObserver2ForPipe::On_1));
		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLSharedExecutiveCommunicationByNamedPipe *psender = new CLSharedExecutiveCommunicationByNamedPipe("test_pipe");
		psender->RegisterSerializer(1, new CLMsg1Serializer);
		psender->RegisterSerializer(2, new CLMsg2Serializer);

		EXPECT_TRUE(CLExecutiveNameServer::GetInstance()->Register("test_pipe", psender).IsSuccess());

		for(int i = 0; i < 10; i++)
		{
			if(i % 2 == 0)
			{
				CLMsg1ForPipe *p = new CLMsg1ForPipe;
				p->i = i;
				p->j = i;

				CLExecutiveNameServer::PostExecutiveMessage("test_pipe", p);
			}
			else
			{
				CLMsg2ForPipe *p = new CLMsg2ForPipe;
				p->i = i;
				p->j = i;

				CLExecutiveNameServer::PostExecutiveMessage("test_pipe", p);
			}
		}

		EXPECT_TRUE(CLExecutiveNameServer::GetInstance()->ReleaseCommunicationPtr("test_pipe").IsSuccess());

		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

class CLTestObserver1ForPipe : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		count = 0;

		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLTestObserver1ForPipe::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLTestObserver1ForPipe::On_2));

		CLThreadForMsgLoop thread1(new CLTestObserver2ForPipe, "test_pipe1");
		EXPECT_TRUE(thread1.Run(0).IsSuccess());

		CLMessage *p = new CLMsg1ForPipe;
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("test_pipe1", p).IsSuccess());

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForPipe *p = dynamic_cast<CLMsg1ForPipe*>(pm);
		EXPECT_TRUE(p != 0);

		count++;

		if(count == 10)
			return CLStatus(QUIT_MESSAGE_LOOP, 0);

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForPipe *p = dynamic_cast<CLMsg2ForPipe*>(pm);
		EXPECT_TRUE(p != 0);

		count++;
		if(count == 10)
			return CLStatus(QUIT_MESSAGE_LOOP, 0);

		return CLStatus(0, 0);
	}
};

TEST(CLMessageQueueByNamedPipe, ProcessPipeQueue_Between_Threads)
{
	CLNonThreadForMsgLoop thread1(new CLTestObserver1ForPipe, "test_pipe", EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE);
	thread1.RegisterDeserializer(1, new CLMsg1Deserializer);
	thread1.RegisterDeserializer(2, new CLMsg2Deserializer);

	EXPECT_TRUE(thread1.Run(0).IsSuccess());

	EXPECT_EQ(count, 10);

	EXPECT_TRUE(true);

	sleep(2);
}

class CLTestObserver3ForPipe : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		count = 0;

		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLTestObserver3ForPipe::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLTestObserver3ForPipe::On_2));

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForPipe *p = dynamic_cast<CLMsg1ForPipe*>(pm);
		EXPECT_TRUE(p != 0);

		count++;

		if(count == 10)
			return CLStatus(QUIT_MESSAGE_LOOP, 0);

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForPipe *p = dynamic_cast<CLMsg2ForPipe*>(pm);
		EXPECT_TRUE(p != 0);

		count++;
		if(count == 10)
			return CLStatus(QUIT_MESSAGE_LOOP, 0);

		return CLStatus(0, 0);
	}
};

TEST(CLMessageQueueByNamedPipe, IN_PROCESS_PIPE_USE)
{
	{
		CLThreadForMsgLoop thread1(new CLTestObserver3ForPipe, "test_pipe", true, EXECUTIVE_IN_PROCESS_USE_PIPE_QUEUE);
		EXPECT_TRUE(thread1.Run(0).IsSuccess());

		for(int i = 0; i < 10; i++)
		{
			if(i % 2 == 0)
			{
				CLMsg1ForPipe *p = new CLMsg1ForPipe;
				p->i = i;
				p->j = i;

				CLExecutiveNameServer::PostExecutiveMessage("test_pipe", p);
			}
			else
			{
				CLMsg2ForPipe *p = new CLMsg2ForPipe;
				p->i = i;
				p->j = i;

				CLExecutiveNameServer::PostExecutiveMessage("test_pipe", p);
			}
		}
	}

	EXPECT_EQ(count, 10);

	EXPECT_TRUE(true);

	sleep(2);
}