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
	}

	long m_i;
	int m_j;
};

class TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsgSerializer : public CLMessageSerializer
{
public:
	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength, unsigned int HeadLength)
	{
		TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg *p = dynamic_cast<TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg *>(pMsg);

		*pFullLength = HeadLength + 8 + 4 + 4;
		char *pBuf = new char[*pFullLength];

		long *pID = (long *)(pBuf + HeadLength);
		*pID = p->m_clMsgID;

		int *pi = (int *)(pBuf + HeadLength + 8);
		*pi = p->m_i;

		int *pj = (int *)(pBuf + HeadLength + 8 + 4);
		*pj = p->m_j;

		return pBuf;
	}
};

class Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsgSerializer : public CLMessageSerializer
{
public:
	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength, unsigned int HeadLength)
	{
		Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg *p = dynamic_cast<Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg *>(pMsg);

		*pFullLength = HeadLength + 8 + 8 + 4;
		char *pBuf = new char[*pFullLength];

		long *pID = (long *)(pBuf + HeadLength);
		*pID = p->m_clMsgID;

		long *pi = (long *)(pBuf + HeadLength + 8);
		*pi = p->m_i;

		int *pj = (int *)(pBuf + HeadLength + 8 + 8);
		*pj = p->m_j;

		return pBuf;
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

		CLSharedExecutiveCommunicationByNamedPipe sender(test_pipe_name);
		sender.RegisterSerializer(1, new TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsgSerializer);
		sender.RegisterSerializer(2, new Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsgSerializer);

		CLMessage *p1 = new TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg;
		CLMessage *p2 = new Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg;
		CLMessage *p3 = new Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg;

		sender.PostExecutiveMessage(p1);
		sender.PostExecutiveMessage(p2);
		sender.PostExecutiveMessage(p3);

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