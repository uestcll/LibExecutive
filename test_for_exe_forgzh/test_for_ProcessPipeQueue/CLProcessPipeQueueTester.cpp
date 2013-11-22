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
	virtual CLStatus Deserialize(CLIOVector &dataVec, CLMessage **pMsg)
	{
		CLMsg1ForCLMsgLoopManagerForPipeQueue *pMsg1 = new CLMsg1ForCLMsgLoopManagerForPipeQueue;

		int *pValue = &(pMsg1->i);

		dataVec.ReadData((char*)(pValue), sizeof(long), sizeof(int));
		pValue = &(pMsg1->j);
		dataVec.ReadData((char*)(pValue), sizeof(long) + sizeof(int), sizeof(int));

		*pMsg = pMsg1;

		return CLStatus(0, 0);
	}

	virtual ~CLMsg1ForCLMsgLoopManagerForPipeQueue_Deserializer()
	{
		g_for_msg1_dese++;
	}
};

class CLMsg2ForCLMsgLoopManagerForPipeQueue_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLStatus Deserialize(CLIOVector &dataVec, CLMessage **pMsg)
	{
		CLMsg2ForCLMsgLoopManagerForPipeQueue *pMsg2 = new CLMsg2ForCLMsgLoopManagerForPipeQueue;

		long *pValue = &(pMsg2->i);

		dataVec.ReadData((char*)(pValue), sizeof(long), sizeof(long));
		int *pValue1 = &(pMsg2->j);
		dataVec.ReadData((char*)(pValue1), sizeof(long) + sizeof(long), sizeof(int));

		*pMsg = pMsg2;

		return CLStatus(0, 0);
	}

	virtual ~CLMsg2ForCLMsgLoopManagerForPipeQueue_Deserializer()
	{
		g_for_msg2_dese++;
	}
};

class CLMsg1ForCLMsgLoopManagerForPipeQueue_Serializer : public CLMessageSerializer
{
public:
	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVector *pDataVec)
	{
		CLMsg1ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg1ForCLMsgLoopManagerForPipeQueue *>(pMsg);		

		int length = sizeof(unsigned long) + sizeof(int) + sizeof(int);
		char *pBuf = new char[length];
		
		unsigned long *pID = (unsigned long *)(pBuf);
		*pID = p->m_clMsgID;

		int *pi = (int *)(pBuf + sizeof(unsigned long));
		*pi = p->i;

		int *pj = (int *)(pBuf + sizeof(unsigned long) + sizeof(int));
		*pj = p->j;

		return (pDataVec->PushBack(pBuf, length));
	}

	virtual ~CLMsg1ForCLMsgLoopManagerForPipeQueue_Serializer()
	{
		g_for_msg1_se++;
	}
};

class CLMsg2ForCLMsgLoopManagerForPipeQueue_Serializer : public CLMessageSerializer
{
public:
	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVector *pDataVec)
	{
		CLMsg2ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg2ForCLMsgLoopManagerForPipeQueue *>(pMsg);		

		int length = sizeof(unsigned long) + sizeof(long) + sizeof(int);
		char *pBuf = new char[length];
		
		unsigned long *pID = (unsigned long *)(pBuf);
		*pID = p->m_clMsgID;

		long *pi = (long *)(pBuf + sizeof(unsigned long));
		*pi = p->i;

		int *pj = (int *)(pBuf + sizeof(unsigned long) + sizeof(long));
		*pj = p->j;

		return (pDataVec->PushBack(pBuf, length));
	}

	virtual ~CLMsg2ForCLMsgLoopManagerForPipeQueue_Serializer()
	{
		g_for_msg2_se++;
	}
};


class CLSharedQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLSharedQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLSharedQueueForSelfPostMsg_CLMsgLoopManagerForPipeQueue::On_2));

		
		// CLSharedExecutiveCommunicationByNamedPipe sender(test_pipe_name);
		CLMessagePoster sender(new CLDataPosterChannelByNamedPipeMaintainer(test_pipe_name, true), new CLMultiMsgSerializer(), new CLProtoEncapForDefaultMsgFormat(), new CLEvent(test_pipe_name, true));
		// EXPECT_TRUE(sender.RegisterSerializer(1, new CLMsg1ForCLMsgLoopManagerForPipeQueue_Serializer).IsSuccess());
		// EXPECT_TRUE(sender.RegisterSerializer(2, new CLMsg2ForCLMsgLoopManagerForPipeQueue_Serializer).IsSuccess());
		sender.Initialize(0);
		sender.RegisterSerializer(1, new CLMsg1ForCLMsgLoopManagerForPipeQueue_Serializer);
		sender.RegisterSerializer(2, new CLMsg2ForCLMsgLoopManagerForPipeQueue_Serializer);

		sender.PostMessage(new CLMsg1ForCLMsgLoopManagerForPipeQueue, new CLDataPostResultNotifier(true));
		sender.PostMessage(new CLMsg2ForCLMsgLoopManagerForPipeQueue,new CLDataPostResultNotifier(true));

		sender.UnInitialize(0);
		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg1ForCLMsgLoopManagerForPipeQueue*>(pm);

		g_for_on1++;

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg2ForCLMsgLoopManagerForPipeQueue*>(pm);

		g_for_on2++;

		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

class CLSharedQueueForProcessPostMsg_CLMsgLoopManagerForPipeQueue : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLSharedQueueForProcessPostMsg_CLMsgLoopManagerForPipeQueue::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLSharedQueueForProcessPostMsg_CLMsgLoopManagerForPipeQueue::On_2));

		CLExecutive *process = new CLProcess(new CLProcessFunctionForExec, true);
		process->Run((void *)"../test_for_processProvider/main");

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg1ForCLMsgLoopManagerForPipeQueue*>(pm);

		g_for_on1++;

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLMsgLoopManagerForPipeQueue *p = dynamic_cast<CLMsg2ForCLMsgLoopManagerForPipeQueue*>(pm);

		g_for_on2++;

		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

int main(int argc, char** agrv)
{
	g_for_on1 = 0;
	g_for_on2 = 0;
	g_for_msg1 = 0;
	g_for_msg2 = 0;
	g_for_msg1_dese = 0;
	g_for_msg2_dese = 0;
	g_for_msg1_se = 0;
	g_for_msg2_se = 0;

	

	if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
   	{
        cout<<"Initialize error"<<endl;
        return 0;
   	}

	CLMsgLoopManagerForPipeQueue *pM = new CLMsgLoopManagerForPipeQueue(new CLSharedQueueForProcessPostMsg_CLMsgLoopManagerForPipeQueue, test_pipe_name, PIPE_QUEUE_BETWEEN_PROCESS);
	pM->RegisterDeserializer(1, new CLMsg1ForCLMsgLoopManagerForPipeQueue_Deserializer);
	pM->RegisterDeserializer(2, new CLMsg2ForCLMsgLoopManagerForPipeQueue_Deserializer);

	SLExecutiveInitialParameter s;
	s.pContext = 0;
	CLThreadInitialFinishedNotifier notifier(0);
	s.pNotifier = &notifier;

	pM->EnterMessageLoop(&s);

	delete pM;

	if(!CLLibExecutiveInitializer::Destroy().IsSuccess())
   	{
            cout<<"Destroy error"<<endl;
            return -1;
   	}

	std::cout<<"(g_for_on1 is , 1) "<< g_for_on1<<std::endl;
	std::cout<<"(g_for_on2 is , 1) "<< g_for_on2<<std::endl;
	std::cout<<"(g_for_msg1 is , 1) "<< g_for_msg1<<std::endl;
	std::cout<<"(g_for_msg2 is , 1) "<< g_for_msg2<<std::endl;
	std::cout<<"(g_for_msg1_dese is , 1) "<< g_for_msg1_dese<<std::endl;
	std::cout<<"(g_for_msg2_dese is , 1) "<< g_for_msg2_dese<<std::endl;
	std::cout<<"(g_for_msg1_se is , 0) "<< g_for_msg1_se<<std::endl;
	std::cout<<"(g_for_msg2_se is , 0) "<< g_for_msg2_se<<std::endl;

	return 0;
}

