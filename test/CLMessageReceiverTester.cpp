#include <gtest/gtest.h>
#include <queue>
#include <fcntl.h>
#include <unistd.h>
#include "LibExecutive.h"

using namespace std;

TEST(CLMessageReceiver, GetMessage_Features_Test)
{
	CLLogger::WriteLogMsg("CLMessageReceiver Test", 0);

	CLSTLqueue stl_queue;
	queue<SLMessageAndSource*> qCon;
	CLUuid u1 = stl_queue.GetUuid();
	CLMessageReceiver *pMsgReceiver = new CLMessageReceiver(&u1, new CLBufferManager(), new CLDataReceiverBySTLqueue(&stl_queue), new CLPointerToMsgDeserializer(), new CLProtocolDecapsulatorBySplitPointer());
	
	CLStatus s1 = pMsgReceiver->GetMessage(qCon);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_TRUE(s1.m_clErrorCode == MSG_RECEIVED_ZERO);
	EXPECT_TRUE(qCon.size() == 1);

	SLMessageAndSource *pInfo = qCon.front();
	qCon.pop();

	CLChannelErrorMsg *p = dynamic_cast<CLChannelErrorMsg *>(pInfo->pMsg);

	EXPECT_TRUE(p->m_clMsgID == MESSAGE_ID_FOR_CHANNEL_ERROR);
	EXPECT_TRUE(p->m_clErrorCode == MSG_RECEIVED_ZERO);

	CLUuidComparer compare;
	EXPECT_TRUE(compare(u1, pInfo->ChannelUuid) == 0);

	delete p;
	delete pInfo;

	delete pMsgReceiver;
}

TEST(CLMessageReceiver, GetMessageSTL_Features_Test)
{
	CLSTLqueue stl_queue;
	queue<SLMessageAndSource *> qCon;
	CLUuid u1 = stl_queue.GetUuid();
	CLMessageReceiver *pMsgReceiver = new CLMessageReceiver(&u1, new CLBufferManager(), new CLDataReceiverBySTLqueue(&stl_queue), new CLPointerToMsgDeserializer(), new CLProtocolDecapsulatorBySplitPointer());

	long i = 0;
	for(; i < 100; i++)
		stl_queue.PushData(i + 1);

	CLStatus s1 = pMsgReceiver->GetMessage(qCon);
	EXPECT_TRUE(s1.IsSuccess());
	EXPECT_TRUE(qCon.size() == 100);
	i = 0;
	while(!qCon.empty())
	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		EXPECT_TRUE(pInfo->pMsg == (CLMessage *)(i + 1));
		i++;

		CLUuidComparer compare;
		EXPECT_TRUE(compare(u1, pInfo->ChannelUuid) == 0);

		delete pInfo;
	}

	i = 0;
	for(; i < 100; i++)
	{
		if(i == 50)
		{
			stl_queue.PushData(0);
			continue;
		}

		stl_queue.PushData(i + 1);
	}

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	CLStatus s2 = pMsgReceiver->GetMessage(qCon);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_TRUE(qCon.size() == 51);

	i = 0;
	while(!qCon.empty())
	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();
	
		if(i == 50)
		{
			CLChannelErrorMsg *p = dynamic_cast<CLChannelErrorMsg *>(pInfo->pMsg);

			EXPECT_TRUE(p->m_clMsgID == MESSAGE_ID_FOR_CHANNEL_ERROR);
			EXPECT_TRUE(p->m_clErrorCode == NORMAL_ERROR);

			delete pInfo->pMsg;

			i++;
		}
		else
		{
			EXPECT_TRUE(pInfo->pMsg == (CLMessage *)(i + 1));
			i++;
		}

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

		delete pInfo;

	}

	for(i = 0; i < 1000; i++)
		stl_queue.PushData(i+1);
	CLStatus s3 = pMsgReceiver->GetMessage(qCon);
	EXPECT_TRUE(s3.IsSuccess());
	EXPECT_TRUE(qCon.size() == 512);
	i = 0;
	while(!qCon.empty())
	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		EXPECT_TRUE(pInfo->pMsg == (CLMessage *)(i + 1));
		i++;

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);
		
		delete pInfo;
	}

	delete pMsgReceiver;
}

TEST(CLMessageReceiver, GetMessageSTL1_Features_Test)
{
	CLSTLqueue stl_queue;
	queue<SLMessageAndSource *> qCon;
	CLUuid u1 = stl_queue.GetUuid();
	CLMessageReceiver *pMsgReceiver = new CLMessageReceiver(&u1, new CLBufferManager(), new CLDataReceiverBySTLqueue(&stl_queue), new CLPointerToMsgDeserializer());

	stl_queue.PushData(1);

	CLStatus s1 = pMsgReceiver->GetMessage(qCon);
	EXPECT_TRUE(s1.IsSuccess());
	EXPECT_TRUE(qCon.size() == 1);

	SLMessageAndSource *pInfo = qCon.front();
	qCon.pop();

	EXPECT_TRUE(pInfo->pMsg == (CLMessage *)1);

	CLUuidComparer compare;
	EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

	delete pInfo;

	delete pMsgReceiver;
}

class CLProtocolDecapsulator_For_CLMessageReceiver_Test : public CLProtocolDecapsulator
{
public:
	CLProtocolDecapsulator_For_CLMessageReceiver_Test() : CLProtocolDecapsulator(0)
	{}
	virtual ~CLProtocolDecapsulator_For_CLMessageReceiver_Test(){}

	virtual CLStatus Decapsulate(CLIOVectors& IOVectorsForData, unsigned int Length, std::vector<CLIOVectors *>& vSerializedMsgs, CLIOVectors& IOVectorsForPartialData, void *pContext)
	{
		return CLStatus(-1, NORMAL_ERROR);
	}

private:
	CLProtocolDecapsulator_For_CLMessageReceiver_Test(const CLProtocolDecapsulator_For_CLMessageReceiver_Test&);
	CLProtocolDecapsulator_For_CLMessageReceiver_Test& operator=(const CLProtocolDecapsulator_For_CLMessageReceiver_Test&);
};

TEST(CLMessageReceiver, GetMessageSTL2_Features_Test)
{
	CLSTLqueue stl_queue;
	queue<SLMessageAndSource *> qCon;
	CLUuid u1 = stl_queue.GetUuid();
	CLMessageReceiver *pMsgReceiver = new CLMessageReceiver(&u1, new CLBufferManager(), new CLDataReceiverBySTLqueue(&stl_queue), new CLPointerToMsgDeserializer(), new CLProtocolDecapsulator_For_CLMessageReceiver_Test());

	stl_queue.PushData(1);

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	CLStatus s1 = pMsgReceiver->GetMessage(qCon);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_TRUE(s1.m_clErrorCode == NORMAL_ERROR);
	EXPECT_TRUE(qCon.size() == 1);

	SLMessageAndSource *pInfo = qCon.front();
	qCon.pop();

	CLChannelErrorMsg *p = dynamic_cast<CLChannelErrorMsg *>(pInfo->pMsg);

	EXPECT_TRUE(p->m_clMsgID == MESSAGE_ID_FOR_CHANNEL_ERROR);
	EXPECT_TRUE(p->m_clErrorCode == NORMAL_ERROR);

	CLUuidComparer compare;
	EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

	delete pInfo->pMsg;
	delete pInfo;

	delete pMsgReceiver;
}


TEST(CLMessageReceiver, GetMessagePrivateNamedPipe_Features_Test)
{
	const char *strPath = "/tmp/NamedPipe_For_CLMessageReceiver_Test";
	queue<SLMessageAndSource *> qCon;
	CLDataReceiver *pTmpDataReveiver = new CLDataReceiverByNamedPipe(strPath);
	CLUuid u1 = pTmpDataReveiver->GetUuid();
	CLMessageReceiver *pMsgReceiver = new CLMessageReceiver(&u1, new CLBufferManager(), pTmpDataReveiver, new CLPointerToMsgDeserializer(), new CLProtocolDecapsulatorBySplitPointer);

	CLStatus s11 = pMsgReceiver->GetMessage(qCon);
	EXPECT_FALSE(s11.IsSuccess());
	EXPECT_TRUE(s11.m_clErrorCode == MSG_RECEIVED_ZERO);

	EXPECT_TRUE(qCon.size() == 1);

	SLMessageAndSource *pInfo = qCon.front();
	qCon.pop();

	CLChannelErrorMsg *p = dynamic_cast<CLChannelErrorMsg *>(pInfo->pMsg);

	EXPECT_TRUE(p->m_clMsgID == MESSAGE_ID_FOR_CHANNEL_ERROR);
	EXPECT_TRUE(p->m_clErrorCode == MSG_RECEIVED_ZERO);

	CLUuidComparer compare1;
	EXPECT_TRUE(compare1(u1, pInfo->ChannelUuid) == 0);

	delete p;
	delete pInfo;

	long i;
	int fd = open(strPath, O_WRONLY);
	for(i = 1; i < 101; i++)
		write(fd, (char *)(&i), sizeof(long));
	close(fd);

	CLStatus s1 = pMsgReceiver->GetMessage(qCon);
	EXPECT_TRUE(s1.IsSuccess());
	EXPECT_TRUE(qCon.size() == 100);
	i = 0;
	while(!qCon.empty())
	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		EXPECT_TRUE(pInfo->pMsg == (CLMessage *)(i + 1));
		i++;

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

		delete pInfo;
	}

	fd = open(strPath, O_WRONLY);
	for(i = 1; i < 101; i++)
	{
		if(i == 51)
		{
			long j = 0;
			write(fd, (char *)(&j), sizeof(long));
			continue;
		}

		write(fd, (char *)(&i), sizeof(long));
	}
	close(fd);

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	CLStatus s2 = pMsgReceiver->GetMessage(qCon);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_TRUE(qCon.size() == 51);
	i = 0;
	while(!qCon.empty())
	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		if(i == 50)
		{

			CLChannelErrorMsg *p = dynamic_cast<CLChannelErrorMsg *>(pInfo->pMsg);

			EXPECT_TRUE(p->m_clMsgID == MESSAGE_ID_FOR_CHANNEL_ERROR);
			EXPECT_TRUE(p->m_clErrorCode == NORMAL_ERROR);

			CLUuidComparer compare;
			EXPECT_TRUE(compare(u1, pInfo->ChannelUuid) == 0);

			delete p;
		}
		else
		{
			EXPECT_TRUE(pInfo->pMsg == (CLMessage *)(i + 1));
		}

		i++;

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

		delete pInfo;
	}

	fd = open(strPath, O_WRONLY);
	for(i = 1; i < 1001; i++)
		write(fd, (char *)(&i), sizeof(long));
	close(fd);

	CLStatus s3 = pMsgReceiver->GetMessage(qCon);
	EXPECT_TRUE(s3.IsSuccess());
	EXPECT_TRUE(qCon.size() == 512);
	i = 0;
	while(!qCon.empty())
	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		EXPECT_TRUE(pInfo->pMsg == (CLMessage *)(i + 1));
		i++;

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

		delete pInfo;
	}
 
	delete pMsgReceiver;
}

TEST(CLMessageReceiver, GetMessagePrivateNamedPipe2_Features_Test)
{
	const char *strPath = "/tmp/NamedPipe_For_CLMessageReceiver_Test";
	queue<SLMessageAndSource *> qCon;
	CLDataReceiver *pTmpDataReveiver = new CLDataReceiverByNamedPipe(strPath);
	CLUuid u1 = pTmpDataReveiver->GetUuid();
	CLMessageReceiver *pMsgReceiver = new CLMessageReceiver(&u1, new CLBufferManager(), pTmpDataReveiver, new CLPointerToMsgDeserializer());

	long i = 1;
	int fd = open(strPath, O_WRONLY);
	write(fd, (char *)(&i), sizeof(long));
	close(fd);

	CLStatus s1 = pMsgReceiver->GetMessage(qCon);
	EXPECT_TRUE(s1.IsSuccess());
	EXPECT_TRUE(qCon.size() == 1);

	SLMessageAndSource *pInfo = qCon.front();
	qCon.pop();
	EXPECT_TRUE(pInfo->pMsg == (CLMessage *)1);

	CLUuidComparer compare;
	EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

	delete pInfo;

	delete pMsgReceiver;
}

TEST(CLMessageReceiver, GetMessagePrivateNamedPipe3_Features_Test)
{
	const char *strPath = "/tmp/NamedPipe_For_CLMessageReceiver_Test";
	queue<SLMessageAndSource *> qCon;
	CLDataReceiver *pTmpDataReveiver = new CLDataReceiverByNamedPipe(strPath);
	CLUuid u1 = pTmpDataReveiver->GetUuid();
	CLMessageReceiver *pMsgReceiver = new CLMessageReceiver(&u1, new CLBufferManager(), pTmpDataReveiver, new CLPointerToMsgDeserializer(), new CLProtocolDecapsulator_For_CLMessageReceiver_Test);

	long i = 1;
	int fd = open(strPath, O_WRONLY);
	write(fd, (char *)(&i), sizeof(long));
	close(fd);

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	CLStatus s1 = pMsgReceiver->GetMessage(qCon);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_TRUE(s1.m_clErrorCode == NORMAL_ERROR);

	EXPECT_TRUE(qCon.size() == 1);

	SLMessageAndSource *pInfo = qCon.front();
	qCon.pop();

	CLChannelErrorMsg *p = dynamic_cast<CLChannelErrorMsg *>(pInfo->pMsg);

	EXPECT_TRUE(p->m_clMsgID == MESSAGE_ID_FOR_CHANNEL_ERROR);
	EXPECT_TRUE(p->m_clErrorCode == NORMAL_ERROR);

	CLUuidComparer compare;
	EXPECT_TRUE(compare(u1, pInfo->ChannelUuid) == 0);

	delete p;
	delete pInfo;

	delete pMsgReceiver;
}

class CLMsg1ForCLMessageReceiverTest : public CLMessage
{
public:
	CLMsg1ForCLMessageReceiverTest() : CLMessage(1)
	{
	}

	virtual ~CLMsg1ForCLMessageReceiverTest()
	{
	}

	int i;
	int j;
};

class CLMsg2ForCLMessageReceiverTest : public CLMessage
{
public:
	CLMsg2ForCLMessageReceiverTest() : CLMessage(2)
	{
	}

	virtual ~CLMsg2ForCLMessageReceiverTest()
	{
	}
};

class CLMsg3ForCLMessageReceiverTest : public CLMessage
{
public:
	CLMsg3ForCLMessageReceiverTest() : CLMessage(3)
	{
	}

	virtual ~CLMsg3ForCLMessageReceiverTest()
	{
	}
};

class CLMsg1ForCLMessageReceiverTest_Deserializer : public CLMessageDeserializer
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

		*ppMsg = new CLMsg1ForCLMessageReceiverTest();

		return CLStatus(0, 0);
	}

	virtual ~CLMsg1ForCLMessageReceiverTest_Deserializer()
	{
	}
};

class CLMsg2ForCLMessageReceiverTest_Deserializer : public CLMessageDeserializer
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

		*ppMsg = new CLMsg2ForCLMessageReceiverTest();

		return CLStatus(0, 0);
	}

	virtual ~CLMsg2ForCLMessageReceiverTest_Deserializer()
	{
	}
};

class CLMsg3ForCLMessageReceiverTest_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLStatus Deserialize(CLIOVectors& IOVectors, CLMessage **ppMsg, CLBufferManager& BufferManager, void *context)
	{
		*ppMsg = 0;

		return CLStatus(-1, NORMAL_ERROR);
	}

	virtual ~CLMsg3ForCLMessageReceiverTest_Deserializer()
	{
	}
};

TEST(CLMessageReceiver, GetMessageNamedPipe_Features_Test)
{
	const char *strPath = "/tmp/NamedPipe_For_CLMessageReceiver_Test";
	queue<SLMessageAndSource *> qCon;
	CLMultiMsgDeserializer *pdd = new CLMultiMsgDeserializer();
	EXPECT_TRUE(pdd->RegisterDeserializer(1, new CLMsg1ForCLMessageReceiverTest_Deserializer).IsSuccess());
	EXPECT_TRUE(pdd->RegisterDeserializer(2, new CLMsg2ForCLMessageReceiverTest_Deserializer).IsSuccess());

	CLDataReceiver *pTmpDataReveiver = new CLDataReceiverByNamedPipe(strPath);
	CLUuid u1 = pTmpDataReveiver->GetUuid();

	CLMessageReceiver *pMsgReceiver = new CLMessageReceiver(&u1, new CLBufferManager(), pTmpDataReveiver, pdd, new CLProtocolDecapsulatorByDefaultMsgFormat);

	CLStatus s11 = pMsgReceiver->GetMessage(qCon);
	EXPECT_FALSE(s11.IsSuccess());
	EXPECT_TRUE(s11.m_clErrorCode == MSG_RECEIVED_ZERO);

	EXPECT_TRUE(qCon.size() == 1);

	SLMessageAndSource *pInfo = qCon.front();
	qCon.pop();

	CLChannelErrorMsg *p = dynamic_cast<CLChannelErrorMsg *>(pInfo->pMsg);

	EXPECT_TRUE(p->m_clMsgID == MESSAGE_ID_FOR_CHANNEL_ERROR);
	EXPECT_TRUE(p->m_clErrorCode == MSG_RECEIVED_ZERO);

	CLUuidComparer compare1;
	EXPECT_TRUE(compare1(u1, pInfo->ChannelUuid) == 0);

	delete p;
	delete pInfo;


	int buf1[5] = {16, 1, 0, 2, 3};
	int buf2[3] = {8, 2, 0};
	int fd = open(strPath, O_WRONLY);
	int i;
	for(i = 1; i < 101; i++)
	{
		if(i % 2 == 0)
			write(fd, (char *)buf1, 20);
		else
			write(fd, (char *)buf2, 12);
	}
	close(fd);

	CLStatus s1 = pMsgReceiver->GetMessage(qCon);
	EXPECT_TRUE(s1.IsSuccess());
	EXPECT_TRUE(qCon.size() == 100);
	i = 1;
	while(!qCon.empty())
	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		if(i % 2 == 0)
		{
			CLMsg1ForCLMessageReceiverTest *p = dynamic_cast<CLMsg1ForCLMessageReceiverTest *>(pInfo->pMsg);
			EXPECT_TRUE(p != 0);
		}
		else
		{
			CLMsg2ForCLMessageReceiverTest *p = dynamic_cast<CLMsg2ForCLMessageReceiverTest *>(pInfo->pMsg);
			EXPECT_TRUE(p != 0);
		}

		i++;

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

		delete pInfo->pMsg;

		delete pInfo;
	}

	fd = open(strPath, O_WRONLY);
	for(i = 1; i < 1001; i++)
	{
		if(i % 2 == 0)
			write(fd, (char *)buf1, 20);
		else
			write(fd, (char *)buf2, 12);
	}
	close(fd);

	for(int j = 0; j < 3; j++)
	{
		CLStatus s3 = pMsgReceiver->GetMessage(qCon);
		EXPECT_TRUE(s3.IsSuccess());
		EXPECT_TRUE(qCon.size() == 256);
		i = 1;
		while(!qCon.empty())
		{
			SLMessageAndSource *pInfo = qCon.front();
			qCon.pop();

			if(i % 2 == 0)
			{
				CLMsg1ForCLMessageReceiverTest *p = dynamic_cast<CLMsg1ForCLMessageReceiverTest *>(pInfo->pMsg);
				EXPECT_TRUE(p != 0);
			}
			else
			{
				CLMsg2ForCLMessageReceiverTest *p = dynamic_cast<CLMsg2ForCLMessageReceiverTest *>(pInfo->pMsg);
				EXPECT_TRUE(p != 0);
			}

			i++;

			CLUuidComparer compare;
			EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

			delete pInfo->pMsg;

			delete pInfo;
		}
	}

	CLStatus s4 = pMsgReceiver->GetMessage(qCon);
	EXPECT_TRUE(s4.IsSuccess());
	EXPECT_TRUE(qCon.size() == 232);
	i = 1;
	while(!qCon.empty())
	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		if(i % 2 == 0)
		{
			CLMsg1ForCLMessageReceiverTest *p = dynamic_cast<CLMsg1ForCLMessageReceiverTest *>(pInfo->pMsg);
			EXPECT_TRUE(p != 0);
		}
		else
		{
			CLMsg2ForCLMessageReceiverTest *p = dynamic_cast<CLMsg2ForCLMessageReceiverTest *>(pInfo->pMsg);
			EXPECT_TRUE(p != 0);
		}

		i++;

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

		delete pInfo->pMsg;

		delete pInfo;
	}

	
	delete pMsgReceiver;
}

TEST(CLMessageReceiver, GetMessageNamedPipe2_Features_Test)
{
	const char *strPath = "/tmp/NamedPipe_For_CLMessageReceiver_Test";
	queue<SLMessageAndSource *> qCon;
	CLMultiMsgDeserializer *pdd = new CLMultiMsgDeserializer();
	EXPECT_TRUE(pdd->RegisterDeserializer(1, new CLMsg1ForCLMessageReceiverTest_Deserializer).IsSuccess());
	EXPECT_TRUE(pdd->RegisterDeserializer(2, new CLMsg2ForCLMessageReceiverTest_Deserializer).IsSuccess());
	EXPECT_TRUE(pdd->RegisterDeserializer(3, new CLMsg3ForCLMessageReceiverTest_Deserializer).IsSuccess());

	CLDataReceiver *pTmpDataReveiver = new CLDataReceiverByNamedPipe(strPath);
	CLUuid u1 = pTmpDataReveiver->GetUuid();

	CLMessageReceiver *pMsgReceiver = new CLMessageReceiver(&u1, new CLBufferManager(), pTmpDataReveiver, pdd, new CLProtocolDecapsulatorByDefaultMsgFormat);

	int buf1[5] = {16, 1, 0, 2, 3};
	int buf2[3] = {8, 2, 0};
	int buf3[3] = {8, 3, 0};
	int fd = open(strPath, O_WRONLY);
	int i;
	for(i = 1; i < 101; i++)
	{
		if(i == 10)
			write(fd, (char *)buf3, 12);
		if(i % 2 == 0)
			write(fd, (char *)buf1, 20);
		else
			write(fd, (char *)buf2, 12);
	}
	close(fd);

	CLLogger::WriteLogMsg("2 The Following bug is produced on purpose 2", 0);
	CLStatus s1 = pMsgReceiver->GetMessage(qCon);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_TRUE(qCon.size() == 10);
	i = 1;
	for(i = 1; i < 10; i++)
	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		if(i % 2 == 0)
		{
			CLMsg1ForCLMessageReceiverTest *p = dynamic_cast<CLMsg1ForCLMessageReceiverTest *>(pInfo->pMsg);
			EXPECT_TRUE(p != 0);
		}
		else
		{
			CLMsg2ForCLMessageReceiverTest *p = dynamic_cast<CLMsg2ForCLMessageReceiverTest *>(pInfo->pMsg);
			EXPECT_TRUE(p != 0);
		}

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

		delete pInfo->pMsg;
		delete pInfo;
	}

	SLMessageAndSource *pErrorInfo = qCon.front();
	qCon.pop();

	CLChannelErrorMsg *p = dynamic_cast<CLChannelErrorMsg *>(pErrorInfo->pMsg);

	EXPECT_TRUE(p->m_clMsgID == MESSAGE_ID_FOR_CHANNEL_ERROR);
	EXPECT_TRUE(p->m_clErrorCode == NORMAL_ERROR);

	CLUuidComparer ErrorCompare;
	EXPECT_TRUE(ErrorCompare(u1, pErrorInfo->ChannelUuid) == 0);

	delete p;
	delete pErrorInfo;

	delete pMsgReceiver;
}

TEST(CLMessageReceiver, GetMessageNamedPipe3_Features_Test)
{
	const char *strPath = "/tmp/NamedPipe_For_CLMessageReceiver_Test";
	queue<SLMessageAndSource *> qCon;
	CLMultiMsgDeserializer *pdd = new CLMultiMsgDeserializer();
	EXPECT_TRUE(pdd->RegisterDeserializer(1, new CLMsg1ForCLMessageReceiverTest_Deserializer).IsSuccess());
	EXPECT_TRUE(pdd->RegisterDeserializer(2, new CLMsg2ForCLMessageReceiverTest_Deserializer).IsSuccess());
	EXPECT_TRUE(pdd->RegisterDeserializer(3, new CLMsg3ForCLMessageReceiverTest_Deserializer).IsSuccess());

	CLDataReceiver *pTmpDataReveiver = new CLDataReceiverByNamedPipe(strPath);
	CLUuid u1 = pTmpDataReveiver->GetUuid();

	CLMessageReceiver *pMsgReceiver = new CLMessageReceiver(&u1, new CLBufferManager(), pTmpDataReveiver, pdd);

	int buf1[5] = {16, 1, 0, 2, 3};
	int fd = open(strPath, O_WRONLY);
	write(fd, (char *)buf1, 20);
	close(fd);

	CLStatus s1 = pMsgReceiver->GetMessage(qCon);
	EXPECT_TRUE(s1.IsSuccess());
	EXPECT_TRUE(qCon.size() == 1);

	SLMessageAndSource *pInfo = qCon.front();
	qCon.pop();

	CLMsg1ForCLMessageReceiverTest *p = dynamic_cast<CLMsg1ForCLMessageReceiverTest *>(pInfo->pMsg);
	EXPECT_TRUE(p != 0);

	CLUuidComparer compare;
	EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

	delete pInfo->pMsg;
	delete pInfo;
	
	delete pMsgReceiver;
}

class CLMsg4ForCLMessageReceiverTest : public CLMessage
{
public:
	CLMsg4ForCLMessageReceiverTest() : CLMessage(4)
	{
	}

	virtual ~CLMsg4ForCLMessageReceiverTest()
	{
	}
};

class CLMsg4ForCLMessageReceiverTest_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLStatus Deserialize(CLIOVectors& IOVectors, CLMessage **ppMsg, CLBufferManager& BufferManager, void *context)
	{
		CLIteratorForIOVectors iter;
		IOVectors.GetIterator(0, iter);

		int len = 0;
		EXPECT_TRUE(IOVectors.ReadBlock(iter, (char *)(&len), 4).IsSuccess());
		EXPECT_EQ(len, 20);

		long id = 0;
		EXPECT_TRUE(IOVectors.ReadBlock(iter, (char *)(&id), 8).IsSuccess());
		EXPECT_EQ(id, 4);

		int x = 0;
		EXPECT_TRUE(IOVectors.ReadBlock(iter, (char *)(&x), 4).IsSuccess());
		EXPECT_EQ(x, 2);

		int y = 0;
		EXPECT_TRUE(IOVectors.ReadBlock(iter, (char *)(&y), 4).IsSuccess());
		EXPECT_EQ(y, 3);

		int z = 0;
		EXPECT_TRUE(IOVectors.ReadBlock(iter, (char *)(&z), 4).IsSuccess());
		EXPECT_EQ(z, 4);

		*ppMsg = new CLMsg4ForCLMessageReceiverTest();

		return CLStatus(0, 0);
	}

	virtual ~CLMsg4ForCLMessageReceiverTest_Deserializer()
	{
	}

	int x;
	int y;
	int z;
};

TEST(CLMessageReceiver, GetMessageNamedPipe4_Features_Test)
{
	const char *strPath = "/tmp/NamedPipe_For_CLMessageReceiver_Test";
	queue<SLMessageAndSource *> qCon;
	CLMultiMsgDeserializer *pdd = new CLMultiMsgDeserializer();
	EXPECT_TRUE(pdd->RegisterDeserializer(1, new CLMsg1ForCLMessageReceiverTest_Deserializer).IsSuccess());
	EXPECT_TRUE(pdd->RegisterDeserializer(4, new CLMsg4ForCLMessageReceiverTest_Deserializer).IsSuccess());

	CLDataReceiver *pTmpDataReveiver = new CLDataReceiverByNamedPipe(strPath);
	CLUuid u1 = pTmpDataReveiver->GetUuid();

	CLMessageReceiver *pMsgReceiver = new CLMessageReceiver(&u1, new CLBufferManager(), pTmpDataReveiver, pdd, new CLProtocolDecapsulatorByDefaultMsgFormat);

	int buf[6] = {20, 4, 0, 2, 3, 4};
	int i;
	int fd = open(strPath, O_WRONLY);
	for(i = 0; i < 1000; i++)
		write(fd, (char *)buf, 24);
	close(fd);

	for(int j = 0; j < 5; j++)
	{
		CLStatus s1 = pMsgReceiver->GetMessage(qCon);
		EXPECT_TRUE(s1.IsSuccess());
		EXPECT_TRUE(qCon.size() == 170);
		while(!qCon.empty())
		{
			SLMessageAndSource *pInfo = qCon.front();
			qCon.pop();

			CLMsg4ForCLMessageReceiverTest *p = dynamic_cast<CLMsg4ForCLMessageReceiverTest *>(pInfo->pMsg);
			EXPECT_TRUE(p != 0);

			CLUuidComparer compare;
			EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

			delete pInfo->pMsg;
			delete pInfo;
		}
	}

	CLStatus s2 = pMsgReceiver->GetMessage(qCon);
	EXPECT_TRUE(s2.IsSuccess());
	EXPECT_TRUE(qCon.size() == 150);
	while(!qCon.empty())
	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		CLMsg4ForCLMessageReceiverTest *p = dynamic_cast<CLMsg4ForCLMessageReceiverTest *>(pInfo->pMsg);
		EXPECT_TRUE(p != 0);

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

		delete pInfo->pMsg;
		delete pInfo;
	}

	delete pMsgReceiver;
}

class CLMsg5ForCLMessageReceiverTest : public CLMessage
{
public:
	CLMsg5ForCLMessageReceiverTest(CLBufferManager *pBM) : CLMessage(5)
	{
		m_pIOV = new CLIOVectors;
		m_pBM = pBM;
	}

	virtual ~CLMsg5ForCLMessageReceiverTest()
	{
		EXPECT_TRUE(m_pBM->ReleaseOccupiedIOVector(*m_pIOV).IsSuccess());
		delete m_pIOV;
	}

	CLIOVectors *m_pIOV;
	CLBufferManager *m_pBM;
};

class CLMsg5ForCLMessageReceiverTest_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLStatus Deserialize(CLIOVectors& IOVectors, CLMessage **ppMsg, CLBufferManager& BufferManager, void *context)
	{
		CLIteratorForIOVectors iter;
		IOVectors.GetIterator(0, iter);

		int len = 0;
		EXPECT_TRUE(IOVectors.ReadBlock(iter, (char *)(&len), 4).IsSuccess());
		EXPECT_EQ(len, 29);

		long id = 0;
		EXPECT_TRUE(IOVectors.ReadBlock(iter, (char *)(&id), 8).IsSuccess());
		EXPECT_EQ(id, 5);

		CLMsg5ForCLMessageReceiverTest *msg = new CLMsg5ForCLMessageReceiverTest(&BufferManager);
		CLStatus s1 = IOVectors.PushBackRangeToAIOVector(*(msg->m_pIOV), iter, 21);
		EXPECT_EQ(s1.m_clReturnCode, 21);
		BufferManager.AddOccupiedIOVector(*(msg->m_pIOV));

		*ppMsg = msg;

		return CLStatus(0, 0);
	}

	virtual ~CLMsg5ForCLMessageReceiverTest_Deserializer()
	{
	}
};

TEST(CLMessageReceiver, GetMessageNamedPipe5_Features_Test)
{
	const char *strPath = "/tmp/NamedPipe_For_CLMessageReceiver_Test";
	queue<SLMessageAndSource *> qCon;
	CLMultiMsgDeserializer *pdd = new CLMultiMsgDeserializer();
	EXPECT_TRUE(pdd->RegisterDeserializer(1, new CLMsg1ForCLMessageReceiverTest_Deserializer).IsSuccess());
	EXPECT_TRUE(pdd->RegisterDeserializer(5, new CLMsg5ForCLMessageReceiverTest_Deserializer).IsSuccess());

	CLDataReceiver *pTmpDataReveiver = new CLDataReceiverByNamedPipe(strPath);
	CLUuid u1 = pTmpDataReveiver->GetUuid();

	CLMessageReceiver *pMsgReceiver = new CLMessageReceiver(&u1, new CLBufferManager(), pTmpDataReveiver, pdd, new CLProtocolDecapsulatorByDefaultMsgFormat);

	int buf[9] = {29, 5, 0, 0, 0, 0, 0, 0, 0};
	int i;
	int fd = open(strPath, O_WRONLY);
	for(i = 0; i < 1000; i++)
		write(fd, (char *)buf, 33);
	close(fd);

	sleep(2);

	i = 0;
	while (true)
	{
		CLStatus s1 = pMsgReceiver->GetMessage(qCon);
		if(s1.m_clErrorCode == MSG_RECEIVED_ZERO)
			break;
			
		if(i == 0)
		{
			EXPECT_EQ(qCon.size(), 124);
		}

		if(i == 1)
		{
			EXPECT_EQ(qCon.size(), 124+45);
		}

		if(i == 2)
		{
			EXPECT_EQ(qCon.size(), 124+45+140);
		}

		if(i == 3)
		{
			EXPECT_EQ(qCon.size(), 124+45+140+51);
		}

		if(i == 4)
		{
			EXPECT_EQ(qCon.size(), 124+45+140+51+143);
		}

		if(i == 5)
		{
			EXPECT_EQ(qCon.size(), 124+45+140+51+143+52);
		}

		if(i == 6)
		{
			EXPECT_EQ(qCon.size(), 124+45+140+51+143+52+143);
		}

		if(i == 7)
		{
			EXPECT_EQ(qCon.size(), 750);
		}

		if(i == 8)
		{
			EXPECT_EQ(qCon.size(), 893);
			break;
		}

		i++;
	}

	i = 0;
	while(!qCon.empty())
	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		CLMsg5ForCLMessageReceiverTest *p = dynamic_cast<CLMsg5ForCLMessageReceiverTest *>(pInfo->pMsg);
		EXPECT_TRUE(p != 0);

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

		delete pInfo->pMsg;
		delete pInfo;

		i++;
	}
	EXPECT_TRUE(i == 893);

	CLStatus s2 = pMsgReceiver->GetMessage(qCon);
	EXPECT_TRUE(s2.IsSuccess());
	EXPECT_EQ(qCon.size(), 1000-893);
	while(!qCon.empty())
	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		CLMsg5ForCLMessageReceiverTest *p = dynamic_cast<CLMsg5ForCLMessageReceiverTest *>(pInfo->pMsg);
		EXPECT_TRUE(p != 0);

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

		delete pInfo->pMsg;
		delete pInfo;
	}

	delete pMsgReceiver;
}

TEST(CLMessageReceiver, GetMessageTCPSocket_Features_Test)
{
	queue<SLMessageAndSource *> qCon;

	CLSocket *pSocket = new CLSocket("3600");
	CLUuid u1 = pSocket->GetUuid();

	CLDataReceiver *pTmpDataReveiver = new CLDataReceiverByAccept(pSocket);

	CLMessageReceiver *pMsgReceiver = new CLMessageReceiver(&u1, new CLBufferManager(), pTmpDataReveiver, new CLClientArrivedMsgDeserializer);

	CLStatus s11 = pMsgReceiver->GetMessage(qCon);
	EXPECT_FALSE(s11.IsSuccess());
	EXPECT_TRUE(s11.m_clErrorCode == MSG_RECEIVED_ZERO);
	
	EXPECT_TRUE(qCon.size() == 1);

	SLMessageAndSource *pErrorInfo = qCon.front();
	qCon.pop();

	CLChannelErrorMsg *p = dynamic_cast<CLChannelErrorMsg *>(pErrorInfo->pMsg);

	EXPECT_TRUE(p->m_clMsgID == MESSAGE_ID_FOR_CHANNEL_ERROR);
	EXPECT_TRUE(p->m_clErrorCode == MSG_RECEIVED_ZERO);

	CLUuidComparer compare1;
	EXPECT_TRUE(compare1(u1, pErrorInfo->ChannelUuid) == 0);

	delete p;
	delete pErrorInfo;

	
	CLSocket *pClientSocket = new CLSocket("127.0.0.1", "3600", SOCKET_TYPE_TCP, true);
	EXPECT_TRUE(pClientSocket->Connect().IsSuccess());

	CLStatus s12 = pMsgReceiver->GetMessage(qCon);
	EXPECT_TRUE(s12.IsSuccess());
	EXPECT_TRUE(qCon.size() == 1);

	CLSocket *pConnSocket;

	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		CLClientArrivedMsg *pMsg = dynamic_cast<CLClientArrivedMsg *>(pInfo->pMsg);
		EXPECT_TRUE(pMsg != 0);

		pConnSocket = pMsg->GetSocket();

		EXPECT_TRUE(pConnSocket != 0);

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

		delete pInfo->pMsg;

		delete pInfo;
	}

	CLMultiMsgDeserializer *pdd = new CLMultiMsgDeserializer();
	EXPECT_TRUE(pdd->RegisterDeserializer(1, new CLMsg1ForCLMessageReceiverTest_Deserializer).IsSuccess());
	EXPECT_TRUE(pdd->RegisterDeserializer(2, new CLMsg2ForCLMessageReceiverTest_Deserializer).IsSuccess());

	CLDataReceiver *pTmpDataReveiver1 = new CLDataReceiverByTCPSocket(pConnSocket);
	CLUuid u2 = pTmpDataReveiver1->GetUuid();
	
	CLMessageReceiver *pMsgReceiver1 = new CLMessageReceiver(&u2, new CLBufferManager(), pTmpDataReveiver1, pdd, new CLProtocolDecapsulatorByDefaultMsgFormat);

	{
		CLStatus s1 = pMsgReceiver1->GetMessage(qCon);
		EXPECT_FALSE(s1.IsSuccess());
		EXPECT_TRUE(s1.m_clErrorCode == MSG_RECEIVED_ZERO);

		EXPECT_TRUE(qCon.size() == 1);

		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		CLChannelErrorMsg *p = dynamic_cast<CLChannelErrorMsg *>(pInfo->pMsg);

		EXPECT_TRUE(p->m_clMsgID == MESSAGE_ID_FOR_CHANNEL_ERROR);
		EXPECT_TRUE(p->m_clErrorCode == MSG_RECEIVED_ZERO);

		CLUuidComparer compare1;
		EXPECT_TRUE(compare1(u2, pInfo->ChannelUuid) == 0);

		delete p;
		delete pInfo;

	}

	int buf1[5] = {16, 1, 0, 2, 3};
	int buf2[3] = {8, 2, 0};
	CLIOVectors iov1, iov2;
	EXPECT_TRUE(iov1.PushBack((char*)buf1, 20).IsSuccess());
	EXPECT_TRUE(iov2.PushBack((char*)buf2, 12).IsSuccess());
	int i;
	for(i = 1; i < 1001; i++)
	{
		if(i % 2 == 0)
		{
			CLStatus s13 = pClientSocket->Write(iov1);
			EXPECT_EQ(s13.m_clReturnCode, 20);
		}
		else
		{
			CLStatus s13 = pClientSocket->Write(iov2);
			EXPECT_EQ(s13.m_clReturnCode, 12);
		}
	}

	while(true)
	{
		CLStatus s1 = pMsgReceiver1->GetMessage(qCon);
		EXPECT_TRUE(s1.IsSuccess());
		if(qCon.size() == 1000)
			break;
	}

	i = 1;
	while(!qCon.empty())
	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		if(i % 2 == 0)
		{
			CLMsg1ForCLMessageReceiverTest *p = dynamic_cast<CLMsg1ForCLMessageReceiverTest *>(pInfo->pMsg);
			EXPECT_TRUE(p != 0);
		}
		else
		{
			CLMsg2ForCLMessageReceiverTest *p = dynamic_cast<CLMsg2ForCLMessageReceiverTest *>(pInfo->pMsg);
			EXPECT_TRUE(p != 0);
		}

		i++;

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u2) == 0);

		delete pInfo->pMsg;

		delete pInfo;
	}
	
	delete pClientSocket;

	sleep(2);

	CLStatus s17 = pMsgReceiver1->GetMessage(qCon);
	EXPECT_TRUE(s17.IsSuccess());
	EXPECT_TRUE(qCon.size() == 1);
	
	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		CLChannelClosedMsg *pMsg = dynamic_cast<CLChannelClosedMsg *>(pInfo->pMsg);
		EXPECT_TRUE(pMsg != 0);

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u2) == 0);

		delete pInfo->pMsg;

		delete pInfo;
	}

	delete pMsgReceiver1;
	delete pMsgReceiver;
}

TEST(CLMessageReceiver, GetMessageTCPSocket_Partial_Features_Test)
{
	queue<SLMessageAndSource *> qCon;

	CLSocket *pSocket = new CLSocket("3600");
	CLUuid u1 = pSocket->GetUuid();

	CLDataReceiver *pTmpDataReveiver = new CLDataReceiverByAccept(pSocket);

	CLMessageReceiver *pMsgReceiver = new CLMessageReceiver(&u1, new CLBufferManager(), pTmpDataReveiver, new CLClientArrivedMsgDeserializer);

	CLSocket *pClientSocket = new CLSocket("127.0.0.1", "3600", SOCKET_TYPE_TCP, true);
	EXPECT_TRUE(pClientSocket->Connect().IsSuccess());

	CLStatus s12 = pMsgReceiver->GetMessage(qCon);
	EXPECT_TRUE(s12.IsSuccess());
	EXPECT_TRUE(qCon.size() == 1);

	CLSocket *pConnSocket;

	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		CLClientArrivedMsg *pMsg = dynamic_cast<CLClientArrivedMsg *>(pInfo->pMsg);
		EXPECT_TRUE(pMsg != 0);

		pConnSocket = pMsg->GetSocket();

		EXPECT_TRUE(pConnSocket != 0);

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

		delete pInfo->pMsg;

		delete pInfo;
	}

	CLMultiMsgDeserializer *pdd = new CLMultiMsgDeserializer();
	EXPECT_TRUE(pdd->RegisterDeserializer(1, new CLMsg1ForCLMessageReceiverTest_Deserializer).IsSuccess());
	EXPECT_TRUE(pdd->RegisterDeserializer(2, new CLMsg2ForCLMessageReceiverTest_Deserializer).IsSuccess());

	CLDataReceiver *pTmpDataReveiver1 = new CLDataReceiverByTCPSocket(pConnSocket);
	CLUuid u2 = pTmpDataReveiver1->GetUuid();

	CLMessageReceiver *pMsgReceiver1 = new CLMessageReceiver(&u2, new CLBufferManager(), pTmpDataReveiver1, pdd, new CLProtocolDecapsulatorByDefaultMsgFormat);

	int buf1[5] = {16, 1, 0, 2, 3};
	int buf2[3] = {8, 2, 0};
	
	CLIOVectors iov1, iov2;
	EXPECT_TRUE(iov1.PushBack((char*)buf1, 20).IsSuccess());
	EXPECT_TRUE(iov2.PushBack((char*)buf2, 12).IsSuccess());
	int i;
	for(i = 1; i < 1001; i++)
	{
		if(i % 2 == 0)
		{
			CLStatus s13 = pClientSocket->Write(iov1);
			EXPECT_EQ(s13.m_clReturnCode, 20);
		}
		else
		{
			CLStatus s13 = pClientSocket->Write(iov2);
			EXPECT_EQ(s13.m_clReturnCode, 12);
		}
	}
	
	CLIOVectors iov3;
	EXPECT_TRUE(iov3.PushBack((char*)buf1, 1).IsSuccess());
	{
		CLStatus s13 = pClientSocket->Write(iov3);
		EXPECT_EQ(s13.m_clReturnCode, 1);
	}

	while(true)
	{
		CLStatus s1 = pMsgReceiver1->GetMessage(qCon);
		EXPECT_TRUE(s1.IsSuccess());
		if(qCon.size() == 1000)
			break;
	}

	i = 1;
	while(!qCon.empty())
	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		if(i % 2 == 0)
		{
			CLMsg1ForCLMessageReceiverTest *p = dynamic_cast<CLMsg1ForCLMessageReceiverTest *>(pInfo->pMsg);
			EXPECT_TRUE(p != 0);
		}
		else
		{
			CLMsg2ForCLMessageReceiverTest *p = dynamic_cast<CLMsg2ForCLMessageReceiverTest *>(pInfo->pMsg);
			EXPECT_TRUE(p != 0);
		}

		i++;

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u2) == 0);

		delete pInfo->pMsg;

		delete pInfo;
	}

	CLIOVectors iov4;
	char *buf11 = (char *)buf1 + 1;
	EXPECT_TRUE(iov4.PushBack(buf11, 19).IsSuccess());
	{
		CLStatus s13 = pClientSocket->Write(iov4);
		EXPECT_EQ(s13.m_clReturnCode, 19);
	}

	sleep(2);

	{
		CLStatus s1 = pMsgReceiver1->GetMessage(qCon);
		EXPECT_TRUE(s1.IsSuccess());
		EXPECT_TRUE(qCon.size() == 1);
		
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		CLMsg1ForCLMessageReceiverTest *p = dynamic_cast<CLMsg1ForCLMessageReceiverTest *>(pInfo->pMsg);
		EXPECT_TRUE(p != 0);

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u2) == 0);

		delete pInfo->pMsg;

		delete pInfo;
	}

	delete pClientSocket;
	delete pMsgReceiver1;
	delete pMsgReceiver;
}

TEST(CLMessageReceiver, GetMessageTCPSocket_DeserialError_Test)
{
	queue<SLMessageAndSource *> qCon;

	CLSocket *pSocket = new CLSocket("3600");
	CLUuid u1 = pSocket->GetUuid();

	CLDataReceiver *pTmpDataReveiver = new CLDataReceiverByAccept(pSocket);

	CLMessageReceiver *pMsgReceiver = new CLMessageReceiver(&u1, new CLBufferManager(), pTmpDataReveiver, new CLClientArrivedMsgDeserializer);

	CLSocket *pClientSocket = new CLSocket("127.0.0.1", "3600", SOCKET_TYPE_TCP, true);
	EXPECT_TRUE(pClientSocket->Connect().IsSuccess());

	CLStatus s12 = pMsgReceiver->GetMessage(qCon);
	EXPECT_TRUE(s12.IsSuccess());
	EXPECT_TRUE(qCon.size() == 1);

	CLSocket *pConnSocket;

	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		CLClientArrivedMsg *pMsg = dynamic_cast<CLClientArrivedMsg *>(pInfo->pMsg);
		EXPECT_TRUE(pMsg != 0);

		pConnSocket = pMsg->GetSocket();

		EXPECT_TRUE(pConnSocket != 0);

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

		delete pInfo->pMsg;

		delete pInfo;
	}

	CLMultiMsgDeserializer *pdd = new CLMultiMsgDeserializer();
	EXPECT_TRUE(pdd->RegisterDeserializer(1, new CLMsg1ForCLMessageReceiverTest_Deserializer).IsSuccess());
	EXPECT_TRUE(pdd->RegisterDeserializer(2, new CLMsg2ForCLMessageReceiverTest_Deserializer).IsSuccess());
	EXPECT_TRUE(pdd->RegisterDeserializer(3, new CLMsg3ForCLMessageReceiverTest_Deserializer).IsSuccess());

	CLDataReceiver *pTmpDataReveiver1 = new CLDataReceiverByTCPSocket(pConnSocket);
	CLUuid u2 = pTmpDataReveiver1->GetUuid();

	CLMessageReceiver *pMsgReceiver1 = new CLMessageReceiver(&u2, new CLBufferManager(), pTmpDataReveiver1, pdd, new CLProtocolDecapsulatorByDefaultMsgFormat);


	int buf1[5] = {16, 1, 0, 2, 3};
	int buf2[3] = {8, 2, 0};
	int buf3[3] = {8, 3, 0};

	CLIOVectors iov1, iov2, iov3;
	EXPECT_TRUE(iov1.PushBack((char*)buf1, 20).IsSuccess());
	EXPECT_TRUE(iov2.PushBack((char*)buf2, 12).IsSuccess());
	EXPECT_TRUE(iov3.PushBack((char*)buf3, 12).IsSuccess());

	int i;
	for(i = 1; i < 101; i++)
	{
		if(i == 10)
		{
			CLStatus s13 = pClientSocket->Write(iov3);
			EXPECT_EQ(s13.m_clReturnCode, 12);
		}
		if(i % 2 == 0)
		{
			CLStatus s13 = pClientSocket->Write(iov1);
			EXPECT_EQ(s13.m_clReturnCode, 20);
		}
		else
		{
			CLStatus s13 = pClientSocket->Write(iov2);
			EXPECT_EQ(s13.m_clReturnCode, 12);
		}
	}

	sleep(2);

	CLLogger::WriteLogMsg("2 The Following bug is produced on purpose 2", 0);
	CLStatus s1 = pMsgReceiver1->GetMessage(qCon);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_TRUE(qCon.size() == 9 + 1);
	i = 1;
	while(!qCon.empty())
	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		if(i == 10)
		{
			CLChannelErrorMsg *p = dynamic_cast<CLChannelErrorMsg *>(pInfo->pMsg);

			EXPECT_TRUE(p->m_clMsgID == MESSAGE_ID_FOR_CHANNEL_ERROR);
			EXPECT_TRUE(p->m_clErrorCode == NORMAL_ERROR);
		}
		else if(i % 2 == 0)
		{
			CLMsg1ForCLMessageReceiverTest *p = dynamic_cast<CLMsg1ForCLMessageReceiverTest *>(pInfo->pMsg);
			EXPECT_TRUE(p != 0);
		}
		else
		{
			CLMsg2ForCLMessageReceiverTest *p = dynamic_cast<CLMsg2ForCLMessageReceiverTest *>(pInfo->pMsg);
			EXPECT_TRUE(p != 0);
		}

		i++;

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u2) == 0);

		delete pInfo->pMsg;
		delete pInfo;
	}

	delete pClientSocket;
	delete pMsgReceiver1;
	delete pMsgReceiver;
}

TEST(CLMessageReceiver, GetMessageTCPSocket_SendPointer_Test)
{
	queue<SLMessageAndSource *> qCon;

	CLSocket *pSocket = new CLSocket("3600");
	CLUuid u1 = pSocket->GetUuid();

	CLDataReceiver *pTmpDataReveiver = new CLDataReceiverByAccept(pSocket);

	CLMessageReceiver *pMsgReceiver = new CLMessageReceiver(&u1, new CLBufferManager(), pTmpDataReveiver, new CLClientArrivedMsgDeserializer);

	CLSocket *pClientSocket = new CLSocket("127.0.0.1", "3600", SOCKET_TYPE_TCP, true);
	EXPECT_TRUE(pClientSocket->Connect().IsSuccess());

	CLStatus s12 = pMsgReceiver->GetMessage(qCon);
	EXPECT_TRUE(s12.IsSuccess());
	EXPECT_TRUE(qCon.size() == 1);

	CLSocket *pConnSocket;

	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		CLClientArrivedMsg *pMsg = dynamic_cast<CLClientArrivedMsg *>(pInfo->pMsg);
		EXPECT_TRUE(pMsg != 0);

		pConnSocket = pMsg->GetSocket();

		EXPECT_TRUE(pConnSocket != 0);

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

		delete pInfo->pMsg;

		delete pInfo;
	}

	CLDataReceiver *pTmpDataReveiver1 = new CLDataReceiverByTCPSocket(pConnSocket);
	CLUuid u2 = pTmpDataReveiver1->GetUuid();

	CLMessageReceiver *pMsgReceiver1 = new CLMessageReceiver(&u2, new CLBufferManager(), pTmpDataReveiver1, new CLPointerToMsgDeserializer(), new CLProtocolDecapsulatorBySplitPointer());

	long i = 0;
	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack((char *)(&i), 8).IsSuccess());
	for(; i < 100; i++)
	{
		long j = i;
		i++;
		CLStatus s13 = pClientSocket->Write(iov);
		EXPECT_EQ(s13.m_clReturnCode, 8);
		i = j;
	}

	sleep(2);

	while(true)
	{
		CLStatus s1 = pMsgReceiver1->GetMessage(qCon);
		EXPECT_TRUE(s1.IsSuccess());

		if(qCon.size() == 100)
			break;
	}
	
	i = 0;
	while(!qCon.empty())
	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		EXPECT_TRUE(pInfo->pMsg == (CLMessage *)(i + 1));
		i++;

		CLUuidComparer compare;
		EXPECT_TRUE(compare(u2, pInfo->ChannelUuid) == 0);

		delete pInfo;
	}

	i = 0;
	for(; i < 100; i++)
	{
		if(i == 50)
		{
			long j = i;
			i = 0;
			CLStatus s13 = pClientSocket->Write(iov);
			EXPECT_EQ(s13.m_clReturnCode, 8);
			i = j;
			continue;
		}

		{
			long j = i;
			i++;
			CLStatus s13 = pClientSocket->Write(iov);
			EXPECT_EQ(s13.m_clReturnCode, 8);
			i = j;
		}
	}

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);

	sleep(2);

	CLStatus s2 = pMsgReceiver1->GetMessage(qCon);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_TRUE(qCon.size() == 50 + 1);

	i = 0;
	while(!qCon.empty())
	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		if(i == 50)
		{
			CLChannelErrorMsg *p = dynamic_cast<CLChannelErrorMsg *>(pInfo->pMsg);

			EXPECT_TRUE(p->m_clMsgID == MESSAGE_ID_FOR_CHANNEL_ERROR);
			EXPECT_TRUE(p->m_clErrorCode == NORMAL_ERROR);

			delete p;
		}
		else
		{
			EXPECT_TRUE(pInfo->pMsg == (CLMessage *)(i + 1));
			i++;
		}

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u2) == 0);

		delete pInfo;
	}

	for(i = 0; i < 1000; i++)
	{
		long j = i;
		i++;
		CLStatus s13 = pClientSocket->Write(iov);
		EXPECT_EQ(s13.m_clReturnCode, 8);
		i = j;
	}

	sleep(2);

	CLStatus s3 = pMsgReceiver1->GetMessage(qCon);
	EXPECT_TRUE(s3.IsSuccess());
	EXPECT_TRUE(qCon.size() == 512);

	i = 0;
	while(!qCon.empty())
	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		EXPECT_TRUE(pInfo->pMsg == (CLMessage *)(i + 1));
		i++;

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u2) == 0);

		delete pInfo;
	}

	delete pClientSocket;
	delete pMsgReceiver1;
	delete pMsgReceiver;
}

TEST(CLMessageReceiver, GetMessageTCPSocket_NONProtocl_Test)
{
	queue<SLMessageAndSource *> qCon;

	CLSocket *pSocket = new CLSocket("3600");
	CLUuid u1 = pSocket->GetUuid();

	CLDataReceiver *pTmpDataReveiver = new CLDataReceiverByAccept(pSocket);

	CLMessageReceiver *pMsgReceiver = new CLMessageReceiver(&u1, new CLBufferManager(), pTmpDataReveiver, new CLClientArrivedMsgDeserializer);

	CLStatus s11 = pMsgReceiver->GetMessage(qCon);
	EXPECT_FALSE(s11.IsSuccess());
	EXPECT_TRUE(s11.m_clErrorCode == MSG_RECEIVED_ZERO);

	EXPECT_TRUE(qCon.size() == 1);

	SLMessageAndSource *pErrorInfo = qCon.front();
	qCon.pop();

	CLChannelErrorMsg *p = dynamic_cast<CLChannelErrorMsg *>(pErrorInfo->pMsg);

	EXPECT_TRUE(p->m_clMsgID == MESSAGE_ID_FOR_CHANNEL_ERROR);
	EXPECT_TRUE(p->m_clErrorCode == MSG_RECEIVED_ZERO);

	CLUuidComparer compare1;
	EXPECT_TRUE(compare1(u1, pErrorInfo->ChannelUuid) == 0);

	delete p;
	delete pErrorInfo;

	CLSocket *pClientSocket = new CLSocket("127.0.0.1", "3600", SOCKET_TYPE_TCP, true);
	EXPECT_TRUE(pClientSocket->Connect().IsSuccess());

	CLStatus s12 = pMsgReceiver->GetMessage(qCon);
	EXPECT_TRUE(s12.IsSuccess());
	EXPECT_TRUE(qCon.size() == 1);

	CLSocket *pConnSocket;

	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		CLClientArrivedMsg *pMsg = dynamic_cast<CLClientArrivedMsg *>(pInfo->pMsg);
		EXPECT_TRUE(pMsg != 0);

		pConnSocket = pMsg->GetSocket();

		EXPECT_TRUE(pConnSocket != 0);

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

		delete pInfo->pMsg;

		delete pInfo;
	}

	CLDataReceiver *pTmpDataReveiver1 = new CLDataReceiverByTCPSocket(pConnSocket);
	CLUuid u2 = pTmpDataReveiver1->GetUuid();

	CLMessageReceiver *pMsgReceiver1 = new CLMessageReceiver(&u2, new CLBufferManager(), pTmpDataReveiver1, new CLMsg1ForCLMessageReceiverTest_Deserializer);

	int buf1[5] = {16, 1, 0, 2, 3};
	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack((char*)buf1, 20).IsSuccess());
	
	{
		CLStatus s13 = pClientSocket->Write(iov);
		EXPECT_EQ(s13.m_clReturnCode, 20);
	}

	sleep(2);

	{
		CLStatus s1 = pMsgReceiver1->GetMessage(qCon);
		EXPECT_TRUE(s1.IsSuccess());
		EXPECT_EQ(qCon.size(), 1);
	}

	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		CLMsg1ForCLMessageReceiverTest *p = dynamic_cast<CLMsg1ForCLMessageReceiverTest *>(pInfo->pMsg);
		EXPECT_TRUE(p != 0);
		
		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u2) == 0);

		delete pInfo->pMsg;

		delete pInfo;
	}

	delete pClientSocket;
	delete pMsgReceiver1;
	delete pMsgReceiver;
}

TEST(CLMessageReceiver, GetMessageTCPSocket_ProtocolError_Test)
{
	queue<SLMessageAndSource *> qCon;

	CLSocket *pSocket = new CLSocket("3600");
	CLUuid u1 = pSocket->GetUuid();

	CLDataReceiver *pTmpDataReveiver = new CLDataReceiverByAccept(pSocket);

	CLMessageReceiver *pMsgReceiver = new CLMessageReceiver(&u1, new CLBufferManager(), pTmpDataReveiver, new CLClientArrivedMsgDeserializer);

	CLStatus s11 = pMsgReceiver->GetMessage(qCon);
	EXPECT_FALSE(s11.IsSuccess());
	EXPECT_TRUE(s11.m_clErrorCode == MSG_RECEIVED_ZERO);

	EXPECT_TRUE(qCon.size() == 1);

	SLMessageAndSource *pErrorInfo = qCon.front();
	qCon.pop();

	CLChannelErrorMsg *p = dynamic_cast<CLChannelErrorMsg *>(pErrorInfo->pMsg);

	EXPECT_TRUE(p->m_clMsgID == MESSAGE_ID_FOR_CHANNEL_ERROR);
	EXPECT_TRUE(p->m_clErrorCode == MSG_RECEIVED_ZERO);

	CLUuidComparer compare1;
	EXPECT_TRUE(compare1(u1, pErrorInfo->ChannelUuid) == 0);

	delete p;
	delete pErrorInfo;

	CLSocket *pClientSocket = new CLSocket("127.0.0.1", "3600", SOCKET_TYPE_TCP, true);
	EXPECT_TRUE(pClientSocket->Connect().IsSuccess());

	CLStatus s12 = pMsgReceiver->GetMessage(qCon);
	EXPECT_TRUE(s12.IsSuccess());
	EXPECT_TRUE(qCon.size() == 1);

	CLSocket *pConnSocket;

	{
		SLMessageAndSource *pInfo = qCon.front();
		qCon.pop();

		CLClientArrivedMsg *pMsg = dynamic_cast<CLClientArrivedMsg *>(pInfo->pMsg);
		EXPECT_TRUE(pMsg != 0);

		pConnSocket = pMsg->GetSocket();

		EXPECT_TRUE(pConnSocket != 0);

		CLUuidComparer compare;
		EXPECT_TRUE(compare(pInfo->ChannelUuid, u1) == 0);

		delete pInfo->pMsg;

		delete pInfo;
	}

	CLDataReceiver *pTmpDataReveiver1 = new CLDataReceiverByTCPSocket(pConnSocket);
	CLUuid u2 = pTmpDataReveiver1->GetUuid();

	CLMessageReceiver *pMsgReceiver1 = new CLMessageReceiver(&u2, new CLBufferManager(), pTmpDataReveiver1, new CLPointerToMsgDeserializer(), new CLProtocolDecapsulator_For_CLMessageReceiver_Test());

	long i = 1;
	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack((char *)(&i), 8).IsSuccess());
	CLStatus s13 = pClientSocket->Write(iov);
	EXPECT_EQ(s13.m_clReturnCode, 8);

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	CLStatus s1 = pMsgReceiver1->GetMessage(qCon);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_TRUE(s1.m_clErrorCode == NORMAL_ERROR);

	EXPECT_TRUE(qCon.size() == 1);

	SLMessageAndSource *pErrorInfo2 = qCon.front();
	qCon.pop();

	CLChannelErrorMsg *pErrMsg = dynamic_cast<CLChannelErrorMsg *>(pErrorInfo2->pMsg);

	EXPECT_TRUE(pErrMsg->m_clMsgID == MESSAGE_ID_FOR_CHANNEL_ERROR);
	EXPECT_TRUE(pErrMsg->m_clErrorCode == NORMAL_ERROR);

	CLUuidComparer compare;
	EXPECT_TRUE(compare(u2, pErrorInfo2->ChannelUuid) == 0);

	delete pErrMsg;
	delete pErrorInfo2;

	delete pClientSocket;
	delete pMsgReceiver1;
	delete pMsgReceiver;
}
