#include <gtest/gtest.h>
#include "LibExecutive.h"

class CLMsg1ForCLMultiMsgDeserializerTest : public CLMessage
{
public:
	CLMsg1ForCLMultiMsgDeserializerTest() : CLMessage(1)
	{
		i = 2;
		j = 3;
	}

	virtual ~CLMsg1ForCLMultiMsgDeserializerTest()
	{
	}

	int i;
	int j;
};

class CLMsg2ForCLMultiMsgDeserializerTest : public CLMessage
{
public:
	CLMsg2ForCLMultiMsgDeserializerTest() : CLMessage(2)
	{
		i = 4;
		j = 6;
	}

	virtual ~CLMsg2ForCLMultiMsgDeserializerTest()
	{
	}

	long i;
	int j;
};

class CLMsg1ForCLMultiMsgDeserializerTest_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLStatus Deserialize(CLIOVectors& IOVectors, CLMessage **ppMsg, CLBufferManager& BufferManager, void *context)
	{
		*ppMsg = (CLMessage *)10;
		return CLStatus(0, 0);
	}

	virtual ~CLMsg1ForCLMultiMsgDeserializerTest_Deserializer()
	{
	}
};

class CLMsg2ForCLMultiMsgDeserializerTest_Deserializer : public CLMessageDeserializer
{
public:
	virtual CLStatus Deserialize(CLIOVectors& IOVectors, CLMessage **ppMsg, CLBufferManager& BufferManager, void *context)
	{
		//*ppMsg = (CLMessage *)20;
		return CLStatus(-1, NORMAL_ERROR);
	}

	virtual ~CLMsg2ForCLMultiMsgDeserializerTest_Deserializer()
	{
	}
};

TEST(CLMultiMsgDeserializer, RegisterDeserializer_Features_Test)
{
	CLLogger::WriteLogMsg("CLMultiMsgDeserializer Test", 0);

	CLMultiMsgDeserializer dd;
	
	CLStatus s1 = dd.RegisterDeserializer(1, 0);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, NORMAL_ERROR);

	CLStatus s2 = dd.RegisterDeserializer(1, new CLMsg1ForCLMultiMsgDeserializerTest_Deserializer);
	EXPECT_TRUE(s2.IsSuccess());

	CLStatus s4 = dd.RegisterDeserializer(2, new CLMsg2ForCLMultiMsgDeserializerTest_Deserializer);
	EXPECT_TRUE(s4.IsSuccess());
	
	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	CLStatus s3 = dd.RegisterDeserializer(1, new CLMsg1ForCLMultiMsgDeserializerTest_Deserializer);
	EXPECT_FALSE(s3.IsSuccess());
	EXPECT_EQ(s3.m_clErrorCode, NORMAL_ERROR);
}

TEST(CLMultiMsgDeserializer, UnregisterDeserializer_Features_Test)
{
	CLMultiMsgDeserializer dd;

	CLStatus s1 = dd.UnregisterDeserializer(1);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, NORMAL_ERROR);

	CLStatus s2 = dd.RegisterDeserializer(1, new CLMsg1ForCLMultiMsgDeserializerTest_Deserializer);
	EXPECT_TRUE(s2.IsSuccess());

	CLStatus s3 = dd.RegisterDeserializer(2, new CLMsg2ForCLMultiMsgDeserializerTest_Deserializer);
	EXPECT_TRUE(s3.IsSuccess());

	CLStatus s4 = dd.UnregisterDeserializer(3);
	EXPECT_FALSE(s4.IsSuccess());
	EXPECT_EQ(s4.m_clErrorCode, NORMAL_ERROR);

	CLStatus s5 = dd.UnregisterDeserializer(1);
	EXPECT_TRUE(s5.IsSuccess());
}

TEST(CLMultiMsgDeserializer, Deserialize_Features_Test)
{
	CLMultiMsgDeserializer dd;

	CLStatus s1 = dd.RegisterDeserializer(1, new CLMsg1ForCLMultiMsgDeserializerTest_Deserializer);
	EXPECT_TRUE(s1.IsSuccess());

	CLStatus s2 = dd.RegisterDeserializer(2, new CLMsg2ForCLMultiMsgDeserializerTest_Deserializer);
	EXPECT_TRUE(s2.IsSuccess());

	CLBufferManager bm;
	CLMessage *pMsg=(CLMessage *)1;
	CLIOVectors iov;

	EXPECT_FALSE(dd.Deserialize(iov, &pMsg, bm, 0).IsSuccess());
	EXPECT_TRUE(pMsg == 0);
	pMsg=(CLMessage *)1;

	char *p1 = new char[1];
	*p1 = 0;

	char *p2 = new char[11];
	for(int i = 0; i < 11; i++)
		p2[i] = 0;

	p2[3] = 1;

	EXPECT_TRUE(iov.PushBack(p1, 1, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p2, 11, true).IsSuccess());
	EXPECT_TRUE(dd.Deserialize(iov, &pMsg, bm, 0).IsSuccess());
	EXPECT_TRUE(pMsg == (CLMessage *)10);

	p2[3] = 2;
	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	EXPECT_FALSE(dd.Deserialize(iov, &pMsg, bm, 0).IsSuccess());
	EXPECT_TRUE(pMsg == (CLMessage *)0);
	pMsg=(CLMessage *)1;

	p2[3] = 3;
	EXPECT_FALSE(dd.Deserialize(iov, &pMsg, bm, 0).IsSuccess());
	EXPECT_TRUE(pMsg == (CLMessage *)0);
}