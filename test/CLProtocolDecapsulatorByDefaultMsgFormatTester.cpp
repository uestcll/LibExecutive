#include <gtest/gtest.h>
#include <vector>
#include "LibExecutive.h"

using namespace std;

static bool CheckIOVectorStatus(iovec *pIOV, int n, CLIOVectors& iovs)
{
	if((pIOV == 0) && (n == 0))
	{
		if((iovs.Size() == 0) &&
			(iovs.GetNumberOfIOVec() == 0) &&
			(iovs.GetIOVecArray() == (iovec *)0))
			return true;
		else
			return false;
	}

	if((pIOV == 0) || (n == 0))
		return false;

	if(iovs.GetNumberOfIOVec() != n)
		return false;

	iovec *piov = iovs.GetIOVecArray();
	if(piov == 0)
		return false;

	try
	{
		int count = 0;

		for(int i = 0; i < n; i++)
		{
			if(pIOV[i].iov_base != piov[i].iov_base)
				throw false;

			if(pIOV[i].iov_len != piov[i].iov_len)
				throw false;

			count += pIOV[i].iov_len;
		}

		if(count != iovs.Size())
			throw false;

		throw true;
	}
	catch(bool b)
	{
		delete [] piov;
		return b;
	}
}

TEST(CLProtocolDecapsulatorByDefaultMsgFormat, Decapsulate_Features_Test)
{
	CLProtocolDecapsulatorByDefaultMsgFormat split;
	CLIOVectors IOVectorsForData;
	CLIOVectors IOVectorsForPartialData;
	vector<CLIOVectors *> vIO;
	long context;

	CLStatus s1 = split.Decapsulate(IOVectorsForData, 0, vIO, IOVectorsForPartialData, &context);
	EXPECT_FALSE(s1.IsSuccess());

	char *p1 = new char[1];
	*p1 = 4;

	char *p2 = new char[10];
	int i;
	for(i = 0; i < 10; i++)
		p2[i] = 0;

	p2[7] = 8;

	char *p3 = new char[5];
	for(i = 0; i < 5; i++)
		p3[i] = 0;

	char *p4 = new char[1];
	*p4 = 0;

	char *p5 = new char[6];
	for(i = 0; i < 6; i++)
		p5[i] = 0;
	p5[3] = 10;

	EXPECT_TRUE(IOVectorsForData.PushBack(p1, 1, true).IsSuccess());
	EXPECT_TRUE(IOVectorsForData.PushBack(p2, 10, true).IsSuccess());
	EXPECT_TRUE(IOVectorsForData.PushBack(p3, 5, true).IsSuccess());
	EXPECT_TRUE(IOVectorsForData.PushBack(p4, 1, true).IsSuccess());
	EXPECT_TRUE(IOVectorsForData.PushBack(p5, 6, true).IsSuccess());

	CLStatus s2 = split.Decapsulate(IOVectorsForData, 100, vIO, IOVectorsForPartialData, &context);
	EXPECT_FALSE(s2.IsSuccess());

	CLStatus s3 = split.Decapsulate(IOVectorsForData, 20, vIO, IOVectorsForPartialData, &context);
	EXPECT_TRUE(s3.IsSuccess());

	iovec IOVEC[6];
	IOVEC[0].iov_base = p1;
	IOVEC[0].iov_len = 1;
	IOVEC[1].iov_base = p2;
	IOVEC[1].iov_len = 7;
	IOVEC[2].iov_base = p2+7;
	IOVEC[2].iov_len = 3;
	IOVEC[3].iov_base = p3;
	IOVEC[3].iov_len = 5;
	IOVEC[4].iov_base = p4;
	IOVEC[4].iov_len = 1;
	IOVEC[5].iov_base = p5;
	IOVEC[5].iov_len = 3;
	EXPECT_EQ(vIO.size(), 2);
	EXPECT_TRUE(CheckIOVectorStatus(IOVEC, 2, *vIO[0]));
	EXPECT_TRUE(CheckIOVectorStatus(&(IOVEC[2]), 4, *vIO[1]));
	EXPECT_TRUE(CheckIOVectorStatus(0, 0, IOVectorsForPartialData));
	for(int i=0; i < vIO.size(); i++)
		delete vIO[i];
	/*
	vIO.clear();
	CLStatus s4 = split.Decapsulate(IOVectorsForData, 23, vIO, IOVectorsForPartialData, &context);
	EXPECT_TRUE(s4.IsSuccess());
	EXPECT_EQ(vIO.size(), 2);
	EXPECT_TRUE(CheckIOVectorStatus(IOVEC, 2, *vIO[0]));
	EXPECT_TRUE(CheckIOVectorStatus(&(IOVEC[2]), 2, *vIO[1]));
	EXPECT_EQ(vIO.size(), 2);

	IOVEC[0].iov_base = p4;
	IOVEC[0].iov_len = 1;
	IOVEC[1].iov_base = p5;
	IOVEC[1].iov_len = 6;
	EXPECT_TRUE(CheckIOVectorStatus(IOVEC, 2, IOVectorsForPartialData));
	for(int i=0; i < vIO.size(); i++)
		delete vIO[i];
*/
}