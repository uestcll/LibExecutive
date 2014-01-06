#include <gtest/gtest.h>
#include "LibExecutive.h"

class CLMsg1ForCLMultiMsgSerializerTest : public CLMessage
{
public:
	CLMsg1ForCLMultiMsgSerializerTest() : CLMessage(1)
	{
		i = 2;
		j = 3;
	}

	virtual ~CLMsg1ForCLMultiMsgSerializerTest()
	{
	}

	int i;
	int j;
};

class CLMsg2ForCLMultiMsgSerializerTest : public CLMessage
{
public:
	CLMsg2ForCLMultiMsgSerializerTest() : CLMessage(2)
	{
		i = 4;
		j = 6;
	}

	virtual ~CLMsg2ForCLMultiMsgSerializerTest()
	{
	}

	long i;
	int j;
};

class CLMsg1ForCLMultiMsgSerializerTest_Serializer : public CLMessageSerializer
{
public:
	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVectors *pIOVectors)
	{
		return CLStatus(0, 0);
	}

	virtual ~CLMsg1ForCLMultiMsgSerializerTest_Serializer()
	{
	}
};

class CLMsg2ForCLMultiMsgSerializerTest_Serializer : public CLMessageSerializer
{
public:
	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVectors *pIOVectors)
	{
		return CLStatus(-1, NORMAL_ERROR);
	}

	virtual ~CLMsg2ForCLMultiMsgSerializerTest_Serializer()
	{
	}
};

TEST(CLMultiMsgSerializer, RegisterSerializer_Features_Test)
{
	CLMultiMsgSerializer dd;
	
	CLStatus s1 = dd.RegisterSerializer(1, 0);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, NORMAL_ERROR);

	CLStatus s2 = dd.RegisterSerializer(1, new CLMsg1ForCLMultiMsgSerializerTest_Serializer);
	EXPECT_TRUE(s2.IsSuccess());

	CLStatus s4 = dd.RegisterSerializer(2, new CLMsg2ForCLMultiMsgSerializerTest_Serializer);
	EXPECT_TRUE(s4.IsSuccess());
	
	CLStatus s3 = dd.RegisterSerializer(1, new CLMsg1ForCLMultiMsgSerializerTest_Serializer);
	EXPECT_FALSE(s3.IsSuccess());
	EXPECT_EQ(s3.m_clErrorCode, NORMAL_ERROR);
}

TEST(CLMultiMsgSerializer, UnregisterSerializer_Features_Test)
{
	CLMultiMsgSerializer dd;

	CLStatus s1 = dd.UnregisterSerializer(1);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, NORMAL_ERROR);

	CLStatus s2 = dd.RegisterSerializer(1, new CLMsg1ForCLMultiMsgSerializerTest_Serializer);
	EXPECT_TRUE(s2.IsSuccess());

	CLStatus s3 = dd.RegisterSerializer(2, new CLMsg2ForCLMultiMsgSerializerTest_Serializer);
	EXPECT_TRUE(s3.IsSuccess());

	CLStatus s4 = dd.UnregisterSerializer(3);
	EXPECT_FALSE(s4.IsSuccess());
	EXPECT_EQ(s4.m_clErrorCode, NORMAL_ERROR);

	CLStatus s5 = dd.UnregisterSerializer(1);
	EXPECT_TRUE(s5.IsSuccess());
}

TEST(CLMultiMsgSerializer, Serialize_Features_Test)
{
	CLMultiMsgSerializer dd;

	CLStatus s1 = dd.RegisterSerializer(1, new CLMsg1ForCLMultiMsgSerializerTest_Serializer);
	EXPECT_TRUE(s1.IsSuccess());

	CLStatus s2 = dd.RegisterSerializer(2, new CLMsg2ForCLMultiMsgSerializerTest_Serializer);
	EXPECT_TRUE(s2.IsSuccess());

	CLMessage *pMsg = new CLMessage(1);
	CLIOVectors iov;
	EXPECT_FALSE(dd.Serialize(0, &iov).IsSuccess());
	EXPECT_FALSE(dd.Serialize(pMsg, 0).IsSuccess());
	EXPECT_TRUE(dd.Serialize(pMsg, &iov).IsSuccess());
	delete pMsg;

	pMsg = new CLMessage(2);
	EXPECT_FALSE(dd.Serialize(pMsg, &iov).IsSuccess());
	delete pMsg;

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	pMsg = new CLMessage(3);
	EXPECT_FALSE(dd.Serialize(pMsg, &iov).IsSuccess());
	delete pMsg;
}