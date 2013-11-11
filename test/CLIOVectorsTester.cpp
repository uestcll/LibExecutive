#include <gtest/gtest.h>
#include "LibExecutive.h"

TEST(CLIOVectors, OriginalStatus)
{
	CLIOVectors iovs;
	EXPECT_EQ(iovs.Size(), 0);
	EXPECT_EQ(iovs.GetNumberOfIOVec(), 0);
	EXPECT_EQ(iovs.GetIOVecArray(), (iovec *)0);

	iovs.Clear();

	CLIteratorForIOVectors it;
	iovs.GetIterator(0, it);
	EXPECT_TRUE(it.IsEnd());
}

TEST(CLIOVectors, ImmediateStatus)
{
	CLIOVectors iovs;

	int *p1 = new int[1];
	*p1 = 0x07060504;
	EXPECT_TRUE(iovs.PushBack((char *)p1, 4, true).IsSuccess());

	int *p2 = new int[1];
	*p2 = 0x0b0a0908;
	EXPECT_TRUE(iovs.PushBack((char *)p2, 4, true).IsSuccess());

	int *p3 = new int[1];
	*p3 = 0x03020100;
	EXPECT_TRUE(iovs.PushFront((char *)p3, 4, true).IsSuccess());

	EXPECT_EQ(iovs.Size(), 12);
	EXPECT_EQ(iovs.GetNumberOfIOVec(), 3);

	iovec *piov = iovs.GetIOVecArray();
	EXPECT_NE(piov, (iovec *)0);

	EXPECT_EQ((char *)piov[0].iov_base, (char *)p3);
	EXPECT_EQ(piov[0].iov_len, 4);
	EXPECT_EQ((char *)piov[1].iov_base, (char *)p1);
	EXPECT_EQ(piov[1].iov_len, 4);
	EXPECT_EQ((char *)piov[2].iov_base, (char *)p2);
	EXPECT_EQ(piov[2].iov_len, 4);

	delete [] piov;
}

TEST(CLIOVectors, PushTest)
{
	CLIOVectors iovs;

	int *p1 = new int[1];
	*p1 = 0x07060504;
	EXPECT_TRUE(iovs.PushBack((char *)p1, 4, true).IsSuccess());

	int *p2 = new int[1];
	*p2 = 0x0b0a0908;
	EXPECT_TRUE(iovs.PushBack((char *)p2, 4, true).IsSuccess());

	int *p3 = new int[1];
	*p3 = 0x03020100;
	EXPECT_TRUE(iovs.PushFront((char *)p3, 4, true).IsSuccess());

	CLIteratorForIOVectors iter;
	iovs.GetIterator(0, iter);

	EXPECT_FALSE(iter.IsEnd());

	int i;
	for(i = 0; !iter.IsEnd(); i++)
	{
		char buf;
		iovs.ReadBlock(iter, &buf, 1);
		EXPECT_EQ((char)i, buf);
	}
	
	EXPECT_EQ(i, 12);

	for(i = 0; i < 12; i++)
	{
		char buf;
		iovs.ReadBlock(i, &buf, 1);
		EXPECT_EQ((char)i, buf);
	}

	EXPECT_EQ(i, 12);
}