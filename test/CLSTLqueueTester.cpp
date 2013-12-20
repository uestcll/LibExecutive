#include <gtest/gtest.h>
#include "LibExecutive.h"

TEST(CLSTLqueue, PushData_Features_Test)
{
	CLSTLqueue q;
	EXPECT_TRUE(q.PushData(2).IsSuccess());

	char *p = (char *)(&q);
	std::queue<unsigned long> *qq = (std::queue<unsigned long> *)(p + 8);
	unsigned long l = qq->front();
	EXPECT_EQ(l, 2);
}

TEST(CLSTLqueue, PopData_Features_Test)
{
	CLSTLqueue q;
	CLIOVectors iov;
	
	CLStatus s1 = q.PopData(iov);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, NORMAL_ERROR);

	char buf[32];
	EXPECT_TRUE(iov.PushBack(buf, 32).IsSuccess());
	CLStatus s2 = q.PopData(iov);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, RECEIVED_ZERO_BYTE);

	for(unsigned long i = 0; i < 10; i++)
		q.PushData(i);

	CLStatus s3 = q.PopData(iov);
	EXPECT_EQ(s3.m_clReturnCode, 32);
	for(int j = 0; j < 32; j++)
	{
		if(j == 8)
			EXPECT_EQ((int)buf[j], 1);
		else if(j == 16)
			EXPECT_EQ((int)buf[j], 2);
		else if(j == 24)
			EXPECT_EQ((int)buf[j], 3);
		else
			EXPECT_EQ((int)buf[j], 0);
	}

	iov.Clear();
	char buf1[20];
	EXPECT_TRUE(iov.PushBack(buf1, 20).IsSuccess());
	CLStatus s4 = q.PopData(iov);
	EXPECT_EQ(s4.m_clReturnCode, 16);
	for(int j = 0; j < 16; j++)
	{
		if(j == 0)
			EXPECT_EQ((int)buf1[j], 4);
		else if(j == 8)
			EXPECT_EQ((int)buf1[j], 5);
		else
			EXPECT_EQ((int)buf1[j], 0);
	}

	iov.Clear();
	char buf2[40];
	EXPECT_TRUE(iov.PushBack(buf2, 40).IsSuccess());
	CLStatus s5 = q.PopData(iov);
	EXPECT_EQ(s5.m_clReturnCode, 32);
	for(int j = 0; j < 32; j++)
	{
		if(j == 0)
			EXPECT_EQ((int)buf2[j], 6);
		else if(j == 8)
			EXPECT_EQ((int)buf2[j], 7);
		else if(j == 16)
			EXPECT_EQ((int)buf2[j], 8);
		else if(j == 24)
			EXPECT_EQ((int)buf2[j], 9);
		else
			EXPECT_EQ((int)buf2[j], 0);
	}
}