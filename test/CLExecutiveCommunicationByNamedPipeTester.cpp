#include <gtest/gtest.h>
#include <fcntl.h>
#include "LibExecutive.h"

static const char *test_pipe_name = "test_for_CLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg";

static int gtestdes = 0;
static int gtest2des = 0;

static bool gtestserial = false;
static bool gtest2serial = false;

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
		gtestdes++;
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
		gtest2des++;
	}

	long m_i;
	int m_j;
};

class TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsgSerializer : public CLMessageSerializer
{
public:
	virtual ~TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsgSerializer()
	{
		gtestserial = true;
	}

	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength, unsigned int HeadLength)
	{
		TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg *p = dynamic_cast<TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg *>(pMsg);
		EXPECT_TRUE(p != 0);

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
	virtual ~Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsgSerializer()
	{
		gtest2serial = true;
	}

	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength, unsigned int HeadLength)
	{
		Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg *p = dynamic_cast<Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg *>(pMsg);
		EXPECT_TRUE(p != 0);

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

TEST(CLExecutiveCommunicationByNamedPipe, PrivateQueueForSelfPostMsg)
{
	string str = "/tmp/";
	str += test_pipe_name;

	mkfifo(str.c_str(), S_IRUSR | S_IWUSR);

	int fd = open(str.c_str(), O_RDONLY | O_NONBLOCK);
	EXPECT_TRUE(fd != -1);

	CLEvent event(test_pipe_name, true);

	CLPrivateExecutiveCommunicationByNamedPipe *sender = new CLPrivateExecutiveCommunicationByNamedPipe(test_pipe_name);
	
	CLMessage *p1 = new TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg;
	CLMessage *p2 = new Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg;
	CLMessage *p3 = new Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg;

	EXPECT_TRUE(sender->PostExecutiveMessage(p1).IsSuccess());
	EXPECT_TRUE(sender->PostExecutiveMessage(p2).IsSuccess());
	EXPECT_TRUE(sender->PostExecutiveMessage(p3).IsSuccess());

	EXPECT_TRUE(event.Wait().IsSuccess());
	CLMessage *pMsg1 = NULL;
	EXPECT_TRUE(read(fd, &pMsg1, sizeof(CLMessage *)) != -1);
	EXPECT_EQ((long)pMsg1, (long)p1);
	delete pMsg1;

	EXPECT_TRUE(event.Wait().IsSuccess());
	CLMessage *pMsg2 = NULL;
	EXPECT_TRUE(read(fd, &pMsg2, sizeof(CLMessage *)) != -1);
	EXPECT_EQ((long)pMsg2, (long)p2);
	delete pMsg2;

	delete sender;

	EXPECT_TRUE(close(fd) != -1);

	EXPECT_EQ(gtest2des, 2);
	EXPECT_EQ(gtestdes, 1);
}

static long ptr_msg1 = 0;
static long ptr_msg2 = 0;
static long ptr_msg3 = 0;

class TestThreadForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg : public CLExecutiveFunctionProvider
{
public:
	virtual CLStatus RunExecutiveFunction(void* pContext)
	{
		CLPrivateExecutiveCommunicationByNamedPipe *psender = (CLPrivateExecutiveCommunicationByNamedPipe *)pContext;

		TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg *p1 = new TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg;
		ptr_msg1 = (long)p1;
		EXPECT_TRUE(psender->PostExecutiveMessage(p1).IsSuccess());

		TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg *p2 = new TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg;
		ptr_msg2 = (long)p2;
		EXPECT_TRUE(psender->PostExecutiveMessage(p2).IsSuccess());		

		TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg *p3 = new TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg;
		ptr_msg3 = (long)p3;
		EXPECT_TRUE(psender->PostExecutiveMessage(p3).IsSuccess());

		return CLStatus(0, 0);
	}
};

TEST(CLExecutiveCommunicationByNamedPipe, PrivateQueueForPostMsg)
{
	string str = "/tmp/";
	str += test_pipe_name;
	mkfifo(str.c_str(), S_IRUSR | S_IWUSR);
	int fd = open(str.c_str(), O_RDONLY | O_NONBLOCK);
	EXPECT_TRUE(fd != -1);

	CLEvent event(test_pipe_name, true);

	{
		CLPrivateExecutiveCommunicationByNamedPipe sender(test_pipe_name);

		CLExecutive *pThread = new CLThread(new TestThreadForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg, true);
		EXPECT_TRUE(pThread->Run(&sender).IsSuccess());

		EXPECT_TRUE(event.Wait().IsSuccess());
		CLMessage *pMsg1 = NULL;
		EXPECT_TRUE(read(fd, &pMsg1, sizeof(CLMessage *)) != -1);
		EXPECT_EQ((long)pMsg1, (long)ptr_msg1);
		delete pMsg1;

		EXPECT_TRUE(event.Wait().IsSuccess());
		CLMessage *pMsg2 = NULL;
		EXPECT_TRUE(read(fd, &pMsg2, sizeof(CLMessage *)) != -1);
		EXPECT_EQ((long)pMsg2, (long)ptr_msg2);
		delete pMsg2;

		EXPECT_TRUE(event.Wait().IsSuccess());
		CLMessage *pMsg3 = NULL;
		EXPECT_TRUE(read(fd, &pMsg3, sizeof(CLMessage *)) != -1);
		EXPECT_EQ((long)pMsg3, (long)ptr_msg3);
		delete pMsg3;

		EXPECT_TRUE(pThread->WaitForDeath().IsSuccess());
	}

	EXPECT_TRUE(close(fd) != -1);
}

TEST(CLExecutiveCommunicationByNamedPipe, SharedQueueForSelfPostMsg)
{
	gtest2des = 0;
	gtestdes = 0;

	string str = "/tmp/";
	str += test_pipe_name;
	mkfifo(str.c_str(), S_IRUSR | S_IWUSR);
	int fd = open(str.c_str(), O_RDONLY | O_NONBLOCK);
	EXPECT_TRUE(fd != -1);

	CLEvent event(test_pipe_name, true);

	{
		CLSharedExecutiveCommunicationByNamedPipe sender(test_pipe_name);
		sender.RegisterSerializer(1, new TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsgSerializer);
		sender.RegisterSerializer(2, new Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsgSerializer);

		CLMessage *p1 = new TestMsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg;
		CLMessage *p2 = new Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg;
		CLMessage *p3 = new Test2MsgForCLExecutiveCommunicationByNamedPipe_PrivateQueueForSelfPostMsg;

		EXPECT_TRUE(sender.PostExecutiveMessage(p1).IsSuccess());
		EXPECT_TRUE(sender.PostExecutiveMessage(p2).IsSuccess());
		EXPECT_TRUE(sender.PostExecutiveMessage(p3).IsSuccess());

		EXPECT_TRUE(event.Wait().IsSuccess());
		char buf1[20];
		EXPECT_TRUE(read(fd, buf1, 20) == 20);
		EXPECT_EQ(*((int *)buf1), 16);
		EXPECT_EQ(*((long *)(buf1 + 4)), 1);
		EXPECT_EQ(*((int *)(buf1 + 4 + 8)), 2);
		EXPECT_EQ(*((int *)(buf1 + 4 + 8 + 4)), 3);

		EXPECT_TRUE(event.Wait().IsSuccess());
		char buf2[24];
		EXPECT_TRUE(read(fd, buf2, 24) == 24);
		EXPECT_EQ(*((int *)buf2), 20);
		EXPECT_EQ(*((long *)(buf2 + 4)), 2);
		EXPECT_EQ(*((int *)(buf2 + 4 + 8)), 4);
		EXPECT_EQ(*((int *)(buf2 + 4 + 8 + 8)), 6);

		EXPECT_TRUE(event.Wait().IsSuccess());
		char buf3[24];
		EXPECT_TRUE(read(fd, buf3, 24) == 24);
		EXPECT_EQ(*((int *)buf3), 20);
		EXPECT_EQ(*((long *)(buf3 + 4)), 2);
		EXPECT_EQ(*((int *)(buf3 + 4 + 8)), 4);
		EXPECT_EQ(*((int *)(buf3 + 4 + 8 + 8)), 6);
	}

	EXPECT_TRUE(close(fd) != -1);

	EXPECT_EQ(gtest2des, 2);
	EXPECT_EQ(gtestdes, 1);
	EXPECT_TRUE(gtest2serial);
	EXPECT_TRUE(gtestserial);
}

TEST(CLExecutiveCommunicationByNamedPipe, SharedQueueForPostMsg)
{
	string str = "/tmp/";
	str += test_pipe_name;
	mkfifo(str.c_str(), S_IRUSR | S_IWUSR);
	int fd = open(str.c_str(), O_RDONLY | O_NONBLOCK);
	EXPECT_TRUE(fd != -1);

	CLEvent event(test_pipe_name, true);

	CLExecutive *process = new CLProcess(new CLProcessFunctionForExec, true);
	EXPECT_TRUE((process->Run((void *)"../test_for_exec/test_for_CLExecutiveCommunicationByNamedPipe/main")).IsSuccess());

	EXPECT_TRUE(event.Wait().IsSuccess());
	char buf1[20];
	EXPECT_TRUE(read(fd, buf1, 20) == 20);
	EXPECT_EQ(*((int *)buf1), 16);
	EXPECT_EQ(*((long *)(buf1 + 4)), 1);
	EXPECT_EQ(*((int *)(buf1 + 4 + 8)), 2);
	EXPECT_EQ(*((int *)(buf1 + 4 + 8 + 4)), 3);

	EXPECT_TRUE(event.Wait().IsSuccess());
	char buf2[24];
	EXPECT_TRUE(read(fd, buf2, 24) == 24);
	EXPECT_EQ(*((int *)buf2), 20);
	EXPECT_EQ(*((long *)(buf2 + 4)), 2);
	EXPECT_EQ(*((int *)(buf2 + 4 + 8)), 4);
	EXPECT_EQ(*((int *)(buf2 + 4 + 8 + 8)), 6);

	EXPECT_TRUE(event.Wait().IsSuccess());
	char buf3[24];
	EXPECT_TRUE(read(fd, buf3, 24) == 24);
	EXPECT_EQ(*((int *)buf3), 20);
	EXPECT_EQ(*((long *)(buf3 + 4)), 2);
	EXPECT_EQ(*((int *)(buf3 + 4 + 8)), 4);
	EXPECT_EQ(*((int *)(buf3 + 4 + 8 + 8)), 6);

	EXPECT_TRUE(process->WaitForDeath().IsSuccess());

	EXPECT_TRUE(close(fd) != -1);
}