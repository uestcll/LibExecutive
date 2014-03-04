#include <iostream>
#include <fcntl.h>
#include "LibExecutive.h"

using namespace std;

class CLMsg1ForCLNonThreadForMsgLoopTest : public CLMessage
{
public:
	CLMsg1ForCLNonThreadForMsgLoopTest() : CLMessage(1)
	{
		i = 2;
		j = 3;
	}

	virtual ~CLMsg1ForCLNonThreadForMsgLoopTest()
	{
	}

	int i;
	int j;
};

class CLMsg2ForCLNonThreadForMsgLoopTest : public CLMessage
{
public:
	CLMsg2ForCLNonThreadForMsgLoopTest() : CLMessage(2)
	{
	}

	virtual ~CLMsg2ForCLNonThreadForMsgLoopTest()
	{
	}
};

class CLMsg1ForCLNonThreadForMsgLoopTest_Serializer : public CLMessageSerializer
{
public:
	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVectors *pIOVectors)
	{
		CLMsg1ForCLNonThreadForMsgLoopTest *pm = dynamic_cast<CLMsg1ForCLNonThreadForMsgLoopTest *>(pMsg);

		char *p = new char [20];

		int *pHead = (int *)p;
		*pHead = 16;

		long *pid = (long *)(p + 4);
		*pid = pm->m_clMsgID;

		int *pi = (int *)(p + 12);
		*pi = pm->i;

		int *pj = (int *)(p + 16);
		*pj = pm->j;

		pIOVectors->PushBack(p, 20, true);

		return CLStatus(0, 0);
	}

	virtual ~CLMsg1ForCLNonThreadForMsgLoopTest_Serializer()
	{
	}
};

class CLMsg2ForCLNonThreadForMsgLoopTest_Serializer : public CLMessageSerializer
{
public:
	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVectors *pIOVectors)
	{
		CLMsg2ForCLNonThreadForMsgLoopTest *pm = dynamic_cast<CLMsg2ForCLNonThreadForMsgLoopTest *>(pMsg);

		char *p = new char [12];

		int *pHead = (int *)p;
		*pHead = 8;

		long *pid = (long *)(p + 4);
		*pid = pm->m_clMsgID;

		pIOVectors->PushBack(p, 12, true);

		return CLStatus(0, 0);
	}

	virtual ~CLMsg2ForCLNonThreadForMsgLoopTest_Serializer()
	{
	}
};

class CLMsg1ForCLNonThreadForMsgLoopTest_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLStatus Deserialize(CLIOVectors& IOVectors, CLMessage **ppMsg, CLBufferManager& BufferManager, void *pContext)
	{
		CLIteratorForIOVectors iter;
		IOVectors.GetIterator(0, iter);

		int len = 0;
		IOVectors.ReadBlock(iter, (char *)(&len), 4);

		long id = 0;
		IOVectors.ReadBlock(iter, (char *)(&id), 8);

		int x = 0;
		IOVectors.ReadBlock(iter, (char *)(&x), 4);

		int y = 0;
		IOVectors.ReadBlock(iter, (char *)(&y), 4);

		*ppMsg = new CLMsg1ForCLNonThreadForMsgLoopTest();

		return CLStatus(0, 0);
	}

	virtual ~CLMsg1ForCLNonThreadForMsgLoopTest_Deserializer()
	{
	}
};

class CLMsg2ForCLNonThreadForMsgLoopTest_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLStatus Deserialize(CLIOVectors& IOVectors, CLMessage **ppMsg, CLBufferManager& BufferManager, void *pContext)
	{
		CLIteratorForIOVectors iter;
		IOVectors.GetIterator(0, iter);

		int len = 0;
		IOVectors.ReadBlock(iter, (char *)(&len), 4);

		long id = 0;
		IOVectors.ReadBlock(iter, (char *)(&id), 8);

		*ppMsg = new CLMsg2ForCLNonThreadForMsgLoopTest();

		return CLStatus(0, 0);
	}

	virtual ~CLMsg2ForCLNonThreadForMsgLoopTest_Deserializer()
	{
	}
};

static const char *test_pipe_name = "test_for_CLNonThreadForMsgLoop_PrivateQueueForSelfPostMsg";
static const char *test_pipe_namepath = "/tmp/test_for_CLNonThreadForMsgLoop_PrivateQueueForSelfPostMsg";
static const char *test1_pipe_name = "test1_for_CLNonThreadForMsgLoop_PrivateQueueForSelfPostMsg";

static int count = 100000;

class CLObserverForCLNonThreadStressTest : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		pMessageLoop->Register(1, (CallBackForMessageLoop)(&CLObserverForCLNonThreadStressTest::On_1));
		pMessageLoop->Register(2, (CallBackForMessageLoop)(&CLObserverForCLNonThreadStressTest::On_2));

		CLExecutiveNameServer::PostExecutiveMessage(test_pipe_name, new CLMsg1ForCLNonThreadForMsgLoopTest, true);

		return CLStatus(0, 0);
	}

	CLStatus On_1(CLMessage *pm)
	{
		CLMsg1ForCLNonThreadForMsgLoopTest *p = dynamic_cast<CLMsg1ForCLNonThreadForMsgLoopTest*>(pm);
		CLMessagePoster *poster = CLExecutiveNameServer::GetInstance()->GetCommunicationPtr(test_pipe_name);

		for(int i = 0; i < count; i++)
		{
			CLProtocolDataPoster *pd = new CLProtocolDataPoster();
			CLStatus s = poster->PostMessage(new CLMsg2ForCLNonThreadForMsgLoopTest, new CLDataPostResultNotifier(true), pd);
			if(!s.IsSuccess())
			{
				if(s.m_clErrorCode == MSG_POST_ERROR)
					break;

				while(true)
				{
					CLStatus s1 = poster->PostLeftMessage(pd);
					if(!s1.IsSuccess())
					{
						if(s1.m_clErrorCode == MSG_POST_ERROR)
							break;

						continue;
					}
					break;
				}
			}
		}

		CLExecutiveNameServer::GetInstance()->ReleaseCommunicationPtr(test_pipe_name);

		return CLStatus(0, 0);
	}

	CLStatus On_2(CLMessage *pm)
	{
		CLMsg2ForCLNonThreadForMsgLoopTest *p = dynamic_cast<CLMsg2ForCLNonThreadForMsgLoopTest*>(pm);

		return CLStatus(QUIT_MESSAGE_LOOP, 0);
	}
};

int main(int argc, char* argv[])
{
	if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
	{
		cout << "Initialize error" << endl;
		return 0;
	}

	CLExecutiveNameServer *pNameServer = 0;

	try
	{
		pNameServer = CLExecutiveNameServer::GetInstance();
		if(pNameServer == 0)
		{
			cout << "pNameServer error" << endl;
			throw CLStatus(-1, 0);
		}

		CLEvent *m_pEvent = new CLEvent(test_pipe_name, true);

		CLMultiMsgSerializer *m_pMsgSerializer = new CLMultiMsgSerializer;
		if(m_pMsgSerializer->RegisterSerializer(1, new CLMsg1ForCLNonThreadForMsgLoopTest_Serializer).IsSuccess() == false)
		{
			cout << "RegisterSerializer error" << endl;
			throw CLStatus(-1, 0);
		}

		if(m_pMsgSerializer->RegisterSerializer(2, new CLMsg2ForCLNonThreadForMsgLoopTest_Serializer).IsSuccess() == false)
		{
			cout << "RegisterSerializer error" << endl;
			throw CLStatus(-1, 0);
		}

		CLMessagePoster *pMsgPoster = new CLMessagePoster(m_pMsgSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(test_pipe_namepath), m_pEvent);
		if(pMsgPoster->Initialize(new CLInitialDataPostChannelNotifier(), 0).IsSuccess() == false)
		{
			cout << "Initialize error" << endl;
			throw CLStatus(-1, 0);
		}

		if(pNameServer->Register(test_pipe_name, pMsgPoster).IsSuccess() == false)
		{
			cout << "name server Register error" << endl;
			throw CLStatus(-1, 0);
		}

		CLNonThreadForMsgLoop thread(new CLObserverForCLNonThreadStressTest, test1_pipe_name, EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE);
		thread.RegisterSerializer(1, new CLMsg1ForCLNonThreadForMsgLoopTest_Serializer);
		thread.RegisterSerializer(2, new CLMsg2ForCLNonThreadForMsgLoopTest_Serializer);

		thread.RegisterDeserializer(1, new CLMsg1ForCLNonThreadForMsgLoopTest_Deserializer);
		thread.RegisterDeserializer(2, new CLMsg2ForCLNonThreadForMsgLoopTest_Deserializer);

		thread.Run((void *)3);
		
		throw CLStatus(0, 0);
	}
	catch(CLStatus& s)
	{
		if(pNameServer != 0)
			pNameServer->ReleaseCommunicationPtr(test_pipe_name);

		if(!CLLibExecutiveInitializer::Destroy().IsSuccess())
			cout << "Destroy error" << endl;

		return 0;
	}

    return 0;
}
