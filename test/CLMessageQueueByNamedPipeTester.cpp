#include <gtest/gtest.h>
#include <fcntl.h>
#include <iostream>
#include "LibExecutive.h"

using namespace std;

static const char *test_pipe_name = "test_for_CLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg";

class TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg : public CLMessage
{
public:
	TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg() : CLMessage(1)
	{
		m_i = 2;
		m_j = 3;
	}

	virtual ~TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg()
	{}

	int m_i;
	int m_j;
};

class Test2MsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg : public CLMessage
{
public:
	Test2MsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg() : CLMessage(2)
	{
		m_i = 4;
		m_j = 6;
	}

	virtual ~Test2MsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg()
	{}

	long m_i;
	int m_j;
};

static bool gdes1 = false;
static bool gdes2 = false;

class TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsgDeserializer : public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg *p = new TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg;
		p->m_i = *((int *)(pBuffer + sizeof(long)));
		p->m_j = *((int *)(pBuffer + sizeof(long) + sizeof(int)));
		return p;
	}

	virtual ~TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsgDeserializer()
	{
		gdes1 = true;
	}
};

class Test2MsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsgDeserializer : public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		Test2MsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg *p = new Test2MsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg;
		p->m_i = *((long *)(pBuffer + sizeof(long)));
		p->m_j = *((int *)(pBuffer + sizeof(long) + sizeof(long)));
		return p;
	}

	virtual ~Test2MsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsgDeserializer()
	{
		gdes2 = true;
	}
};

TEST(CLMessageQueueByNamedPipe, PrivateQueueForSelfPostMsg)
{
	CLPrivateMsgQueuebyNamedPipe private_msg_queue(test_pipe_name);

	string str = "/tmp/";
	str += test_pipe_name;
	int fd = open(str.c_str(), O_WRONLY | O_NONBLOCK);
	EXPECT_TRUE(fd != -1);

	TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg *p = new TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg;
	EXPECT_TRUE(write(fd, &p, sizeof(long)) != -1);

	{
		CLEvent event(test_pipe_name, true);
		event.Set();
	}

	close(fd);

	CLMessage *q = private_msg_queue.GetMessage();
	EXPECT_TRUE((long)q == (long)p);
	delete p;
}

static long ptr_msg1 = 0;
static long ptr_msg2 = 0;
static long ptr_msg3 = 0;

class TestThreadForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg : public CLExecutiveFunctionProvider
{
public:
	virtual CLStatus RunExecutiveFunction(void* pContext)
	{
		string str = "/tmp/";
		str += test_pipe_name;
		int fd = open(str.c_str(), O_WRONLY | O_NONBLOCK);
		EXPECT_TRUE(fd != -1);

		TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg *p1 = new TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg;
		ptr_msg1 = (long)p1;
		EXPECT_TRUE(write(fd, &p1, sizeof(long)) != -1);
		{
			CLEvent event(test_pipe_name, true);
			event.Set();
		}

		TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg *p2 = new TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg;
		ptr_msg2 = (long)p2;
		EXPECT_TRUE(write(fd, &p2, sizeof(long)) != -1);
		{
			CLEvent event(test_pipe_name, true);
			event.Set();
		}

		TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg *p3 = new TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg;
		ptr_msg3 = (long)p3;
		EXPECT_TRUE(write(fd, &p3, sizeof(long)) != -1);
		{
			CLEvent event(test_pipe_name, true);
			event.Set();
		}

		close(fd);

		return CLStatus(0, 0);
	}
};

TEST(CLMessageQueueByNamedPipe, PrivateQueueForPostMsg)
{
	CLPrivateMsgQueuebyNamedPipe private_msg_queue(test_pipe_name);

	CLExecutive *pThread = new CLThread(new TestThreadForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg, true);

	EXPECT_TRUE(pThread->Run().IsSuccess());

	CLMessage *q1 = private_msg_queue.GetMessage();
	EXPECT_TRUE((long)q1 == ptr_msg1);
	delete q1;

	CLMessage *q2 = private_msg_queue.GetMessage();
	EXPECT_TRUE((long)q2 == ptr_msg2);
	delete q2;

	CLMessage *q3 = private_msg_queue.GetMessage();
	EXPECT_TRUE((long)q3 == ptr_msg3);
	delete q3;

	EXPECT_TRUE(pThread->WaitForDeath().IsSuccess());
}

TEST(CLMessageQueueByNamedPipe, SharedQueueForSelfPostMsg)
{
	{
		CLSharedMsgQueueByNamedPipe shared_msg_queue(test_pipe_name);
		EXPECT_TRUE(shared_msg_queue.RegisterDeserializer(1, new TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsgDeserializer).IsSuccess());
		EXPECT_TRUE(shared_msg_queue.RegisterDeserializer(2, new Test2MsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsgDeserializer).IsSuccess());

		string str = "/tmp/";
		str += test_pipe_name;
		int fd = open(str.c_str(), O_WRONLY | O_NONBLOCK);
		EXPECT_TRUE(fd != -1);

		char *p = new char[20];
		*((int *)p) = 16;
		*((long *)(p + 4)) = 1;
		*((int *)(p + 12)) = 2;
		*((int *)(p + 16)) = 3;
		EXPECT_TRUE(write(fd, p, 20) != -1);
		{
			CLEvent event(test_pipe_name, true);
			event.Set();
		}
		delete [] p;

		p = new char[24];
		*((int *)p) = 20;
		*((long *)(p + 4)) = 2;
		*((long *)(p + 12)) = 4;
		*((int *)(p + 20)) = 6;
		EXPECT_TRUE(write(fd, p, 24) != -1);
		{
			CLEvent event(test_pipe_name, true);
			event.Set();
		}
		delete [] p;

		close(fd);

		CLMessage *q = shared_msg_queue.GetMessage();
		TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg *q1 = dynamic_cast<TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg *>(q);
		EXPECT_TRUE(q1 != 0);
		EXPECT_EQ(q1->m_i, 2);
		EXPECT_EQ(q1->m_j, 3);
		delete q;

		q = shared_msg_queue.GetMessage();
		Test2MsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg *q2 = dynamic_cast<Test2MsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg *>(q);
		EXPECT_TRUE(q2 != 0);
		EXPECT_EQ(q2->m_i, 4);
		EXPECT_EQ(q2->m_j, 6);
		delete q;
	}

	EXPECT_TRUE(gdes1);
	EXPECT_TRUE(gdes2);
}

TEST(CLMessageQueueByNamedPipe, SharedQueueForPostMsg)
{
	CLSharedMsgQueueByNamedPipe shared_msg_queue(test_pipe_name);
	EXPECT_TRUE(shared_msg_queue.RegisterDeserializer(1, new TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsgDeserializer).IsSuccess());
	EXPECT_TRUE(shared_msg_queue.RegisterDeserializer(2, new Test2MsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsgDeserializer).IsSuccess());

	CLExecutive *process = new CLProcess(new CLProcessFunctionForExec, true);
	EXPECT_TRUE((process->Run((void *)"../test_for_exec/test_for_CLMessageQueueByNamedPipe/main")).IsSuccess());

	CLMessage *q = shared_msg_queue.GetMessage();
	TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg *q1 = dynamic_cast<TestMsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg *>(q);
	EXPECT_TRUE(q1 != 0);
	EXPECT_EQ(q1->m_i, 2);
	EXPECT_EQ(q1->m_j, 3);
	delete q;

	q = shared_msg_queue.GetMessage();
	Test2MsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg *q2 = dynamic_cast<Test2MsgForCLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg *>(q);
	EXPECT_TRUE(q2 != 0);
	EXPECT_EQ(q2->m_i, 4);
	EXPECT_EQ(q2->m_j, 6);
	delete q;

	EXPECT_TRUE(process->WaitForDeath().IsSuccess());
}