#include <iostream>
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

class CLQuitMessage : public CLMessage
{
public:
	CLQuitMessage() : CLMessage(100)
	{}
	virtual ~CLQuitMessage()
	{}
};

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
		pMessageLoop->Register(100, (CallBackForMessageLoop)(&CLPrivateQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue::On_Quit));

		CLMessage *p = new CLMsg1ForCLMsgLoopManagerForPipeQueue;
		// EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p).IsSuccess());
		CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p);

		p = new CLMsg2ForCLMsgLoopManagerForPipeQueue;
		// EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p).IsSuccess());
		CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p);

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg1ForCLMsgLoopManagerForPipeQueue*>(pm);
		if(p == 0)
		{
			std::cout<<"In CLPrivateQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue,  On_1(), the p == 0"<<std::endl;
			return CLStatus(-1, 0);
		}

		g_for_on1++;

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg2ForCLMsgLoopManagerForPipeQueue*>(pm);
		if(p == 0)
		{
			std::cout<<"In CLPrivateQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue,  On_2(), the p == 0"<<std::endl;
			return CLStatus(-1, 0);
		}

		g_for_on2++;

		CLMessage *p1 = new CLMsg2ForCLMsgLoopManagerForPipeQueue;
		// EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p1).IsSuccess());
		CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, p1);
		CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, new CLQuitMessage());

		return CLStatus(0, 0);
	}

	CLStatus On_Quit(CLMessage *pm)
	{
		std::cout<<"Quit!"<<std::endl;
		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

int main()
{
    if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
   	{
        cout<<"Initialize error"<<endl;
        return 0;
   	}

	CLMessageLoopManager *pM = new CLMsgLoopManagerForPipeQueue(new CLPrivateQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue, test_pipe_name, PIPE_QUEUE_IN_PROCESS);

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	// EXPECT_TRUE((pM->EnterMessageLoop(&s)).IsSuccess());
	pM->EnterMessageLoop(&s);

	delete pM;

	std::cout<<"the g_for_on1 (1) is"<<g_for_on1<<std::endl;
	std::cout<<"the g_for_on2 (1) is"<<g_for_on2<<std::endl;
	std::cout<<"the g_for_msg1 (1) is"<<g_for_msg1<<std::endl;
	std::cout<<"the g_for_msg2 (2) is"<<g_for_msg2<<std::endl;

	if(!CLLibExecutiveInitializer::Destroy().IsSuccess())
   	{
            cout<<"Destroy error"<<endl;
            return -1;
   	}
	// EXPECT_EQ(g_for_on1, 1);
	// EXPECT_EQ(g_for_on2, 1);
	// EXPECT_EQ(g_for_msg1, 1);
	// EXPECT_EQ(g_for_msg2, 2);

	return 0;
}