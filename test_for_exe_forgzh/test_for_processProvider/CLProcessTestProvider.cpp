#include <iostream>
#include <fcntl.h>
#include "LibExecutive.h"

using namespace std;

static const char *test_pipe_name = "test_for_CLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg";

class TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg : public CLMessage
{
public:
	TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg() : CLMessage(1)
	{
		m_i = 2;
		m_j = 3;
	}

	virtual ~TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg()
	{
		std::cout<<"in testmsg 1 ~"<<endl;
	}

	int m_i;
	int m_j;
};

class Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg : public CLMessage
{
public:
	Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg() : CLMessage(2)
	{
		m_i = 4;
		m_j = 6;
	}

	virtual ~Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg()
	{
		std::cout<<"in testmsg 2 ~"<<endl;
	}

	long m_i;
	int m_j;
};

class TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsgSerializer : public CLMessageSerializer
{
public:
	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVector *pDataVec)
	{
		TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg *p = dynamic_cast<TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg *>(pMsg);

		int length = sizeof(unsigned long) + sizeof(int) + sizeof(int);
		char *pBuf = new char[length];
		
		unsigned long *pID = (unsigned long *)(pBuf);
		*pID = p->m_clMsgID;

		int *pi = (int *)(pBuf + sizeof(unsigned long));
		*pi = p->m_i;

		int *pj = (int *)(pBuf + sizeof(unsigned long) + sizeof(int));
		*pj = p->m_j;

		return (pDataVec->PushBack(pBuf, length));
	}

	virtual ~TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsgSerializer()
	{
		std::cout<<"in msg 1 Serialize ~"<<endl;
	}
	
};

class Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsgSerializer : public CLMessageSerializer
{
public:
	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVector *pDataVec)
	{
		Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg *p = dynamic_cast<Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg *>(pMsg);
		
		int length = sizeof(unsigned long) + sizeof(long) + sizeof(int);
		char *pBuf = new char[length];
		
		unsigned long *pID = (unsigned long *)(pBuf);
		*pID = p->m_clMsgID;

		long *pi = (long *)(pBuf + sizeof(unsigned long));
		*pi = p->m_i;

		int *pj = (int *)(pBuf + sizeof(unsigned long) + sizeof(long));
		*pj = p->m_j;

		return (pDataVec->PushBack(pBuf, length));
	}

	virtual ~Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsgSerializer()
	{
			std::cout<<"in msg 2 Serialize ~"<<endl;
	}
	
};

int main(int argc, char* argv[])
{
	try
	{
		if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
		{
			cout << "Initialize error" << endl;
			return 0;
		}

		CLMessagePoster sender(new CLDataPosterChannelByNamedPipeMaintainer(test_pipe_name, true), new CLMultiMsgSerializer(), new CLProtoEncapForDefaultMsgFormat(), new CLEvent(test_pipe_name, true));

		sender.Initialize(0);
		sender.RegisterSerializer(1, new TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsgSerializer);
		sender.RegisterSerializer(2, new Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsgSerializer);

		sender.PostMessage(new TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg, new CLDataPostResultNotifier(true));
		sender.PostMessage(new Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg,new CLDataPostResultNotifier(true));

		sender.UnInitialize(0);

		throw CLStatus(0, 0);
	}
	catch(CLStatus& s)
	{
		if(!CLLibExecutiveInitializer::Destroy().IsSuccess())
			cout << "Destroy error" << endl;

		return 0;
	}

    return 0;
}