#include <gtest/gtest.h>
#include "LibExecutive.h"

bool CheckIOVectorStatus(iovec *pIOV, int n, CLIOVectors& iovs)
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

bool CheckIOVectorData(char *buf, int n, CLIOVectors& iovs)
{
	CLIteratorForIOVectors iter;
	iovs.GetIterator(0, iter);

	if((buf == 0) && (n == 0))
	{
		if(iter.IsEnd())
			return true;
		else
			return false;
	}

	if((buf == 0) || (n == 0))
		return false;

	if(n != iovs.Size())
		return false;

	for(int i = 0; i < n; i++)
	{
		char c;
		iovs.ReadBlock(iter, &c, 1);

		if(c != buf[i])
			return false;
	}

	return true;
}

TEST(CLIOVectors, OriginalStatus)
{
	CLIOVectors iovs;
	EXPECT_TRUE(CheckIOVectorStatus(0, 0, iovs));

	iovs.Clear();

	CLIteratorForIOVectors it;
	iovs.GetIterator(0, it);
	EXPECT_TRUE(it.IsEnd());
}

TEST(CLIOVectors, PushBackAndFront_Para_Test)
{
	CLIOVectors iovs;
	EXPECT_FALSE(iovs.PushBack(0, 2).IsSuccess());
	EXPECT_FALSE(iovs.PushBack(0, 0).IsSuccess());
	EXPECT_FALSE(iovs.PushBack((char *)2, 0).IsSuccess());
}

TEST(CLIOVectors, PushBackAndFront_Features_Test)
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

	iovec IOVEC[3];
	IOVEC[0].iov_base = p3;
	IOVEC[0].iov_len = 4;
	IOVEC[1].iov_base = p1;
	IOVEC[1].iov_len = 4;
	IOVEC[2].iov_base = p2;
	IOVEC[2].iov_len = 4;

	EXPECT_TRUE(CheckIOVectorStatus(IOVEC, 3, iovs));

	char buf[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb};
	EXPECT_TRUE(CheckIOVectorData(buf, 12, iovs));
}

TEST(CLIOVectors, PopBackAndFront_Para_Test)
{
	CLIOVectors iovs;
	EXPECT_FALSE(iovs.PopBack(0, (size_t *)2).IsSuccess());
	EXPECT_FALSE(iovs.PopBack(0, 0).IsSuccess());
	EXPECT_FALSE(iovs.PopFront((char **)2, 0).IsSuccess());

	char *p;
	size_t n;
	EXPECT_FALSE(iovs.PopBack(&p, &n).IsSuccess());
	EXPECT_TRUE(p == 0);
	EXPECT_EQ(n, 0);
}

TEST(CLIOVectors, PopBackAndFront_Features_Test)
{
	int i;
	CLIOVectors iovs;

	char *p1 = new char[10];
	for(i = 0; i < 10; i++)
		p1[i] = i;

	char *p2 = new char[20];
	for(i = 0; i < 20; i++)
		p2[i] = i * 2;

	char *p3 = new char[30];
	for(i = 0; i < 30; i++)
		p3[i] = i * 3;

	EXPECT_TRUE(iovs.PushBack(p1, 10, true).IsSuccess());
	EXPECT_TRUE(iovs.PushBack(p2, 20, true).IsSuccess());
	EXPECT_TRUE(iovs.PushFront(p3, 30, true).IsSuccess());

	char *p4;
	size_t n;
	EXPECT_TRUE(iovs.PopFront(&p4, &n).IsSuccess());
	EXPECT_TRUE(p3 == p4);
	EXPECT_TRUE(n == 30);
	for(i = 0; i < 30; i++)
	{
		EXPECT_EQ(p4[i], i * 3);
	}

	delete [] p3;

	EXPECT_TRUE(iovs.PopBack(&p4, &n).IsSuccess());
	EXPECT_TRUE(p2 == p4);
	EXPECT_TRUE(n == 20);
	for(i = 0; i < 20; i++)
	{
		EXPECT_EQ(p4[i], i * 2);
	}

	delete [] p2;

	iovec IOVEC[1];
	IOVEC[0].iov_base = p1;
	IOVEC[0].iov_len = 10;

	EXPECT_TRUE(CheckIOVectorStatus(IOVEC, 1, iovs));

	char buf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	EXPECT_TRUE(CheckIOVectorData(buf, 10, iovs));
}

bool CheckCLIOVectorIterator(char *pData, CLIteratorForIOVectors& iter)
{
	long *p = (long *)(&iter);
	p++;
	if((char *)(*p) == pData)
		return true;
	else
		return false;
}

TEST(CLIOVectors, GetIterator_Features_Test)
{
	CLIOVectors iov;

	CLIteratorForIOVectors iter1;
	iov.GetIterator(0, iter1);
	EXPECT_TRUE(iter1.IsEnd());
	EXPECT_TRUE(CheckCLIOVectorIterator(0, iter1));

	CLIteratorForIOVectors iter2;
	iov.GetIterator(100, iter2);
	EXPECT_TRUE(iter2.IsEnd());
	EXPECT_TRUE(CheckCLIOVectorIterator(0, iter2));

	char *p1 = new char[1];
	*p1 = 2;
	EXPECT_TRUE(iov.PushBack(p1, 1, true).IsSuccess());
	CLIteratorForIOVectors iter3;
	iov.GetIterator(0, iter3);
	EXPECT_FALSE(iter3.IsEnd());
	EXPECT_TRUE(CheckCLIOVectorIterator(p1, iter3));

	CLIteratorForIOVectors iter4;
	iov.GetIterator(100, iter4);
	EXPECT_TRUE(iter4.IsEnd());
	EXPECT_TRUE(CheckCLIOVectorIterator(0, iter4));

	char *p2 = new char[10];
	int i;
	for(i = 0; i < 10; i++)
		p2[i] = i;
	EXPECT_TRUE(iov.PushBack(p2, 10, true).IsSuccess());

	CLIteratorForIOVectors iter5;
	iov.GetIterator(0, iter5);
	EXPECT_FALSE(iter5.IsEnd());
	EXPECT_TRUE(CheckCLIOVectorIterator(p1, iter5));

	CLIteratorForIOVectors iter6;
	iov.GetIterator(6, iter6);
	EXPECT_FALSE(iter6.IsEnd());
	EXPECT_TRUE(CheckCLIOVectorIterator(p2+5, iter6));

	CLIteratorForIOVectors iter7;
	iov.GetIterator(100, iter7);
	EXPECT_TRUE(iter7.IsEnd());
	EXPECT_TRUE(CheckCLIOVectorIterator(0, iter7));

	char *p3 = new char[5];
	for(i = 0; i < 5; i++)
		p3[i] = i * 5;
	EXPECT_TRUE(iov.PushBack(p3, 5, true).IsSuccess());

	char *p4 = new char[1];
	*p4 = 0;
	EXPECT_TRUE(iov.PushBack(p4, 1, true).IsSuccess());

	char *p5 = new char[6];
	for(i = 0; i < 6; i++)
		p5[i] = i;
	EXPECT_TRUE(iov.PushBack(p5, 6, true).IsSuccess());

	CLIteratorForIOVectors iter8;
	iov.GetIterator(16, iter8);
	EXPECT_FALSE(iter8.IsEnd());
	EXPECT_TRUE(CheckCLIOVectorIterator(p4, iter8));

	CLIteratorForIOVectors iter9;
	iov.GetIterator(22, iter9);
	EXPECT_FALSE(iter9.IsEnd());
	EXPECT_TRUE(CheckCLIOVectorIterator(p5+5, iter9));

	CLIteratorForIOVectors iter10;
	iov.GetIterator(23, iter10);
	EXPECT_TRUE(iter10.IsEnd());
	EXPECT_TRUE(CheckCLIOVectorIterator(0, iter10));
}

TEST(CLIOVectors, Iterator_Features_Test)
{
	CLIOVectors iov;

	CLIteratorForIOVectors iter1;
	iov.GetIterator(0, iter1);
	CLStatus s1 = iter1.Add(3);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, NORMAL_ERROR);

	CLIteratorForIOVectors iter2;
	iov.GetIterator(0, iter2);
	CLStatus s2 = iter1.Sub(1);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, NORMAL_ERROR);

	char *p1 = new char[1];
	*p1 = 2;
	EXPECT_TRUE(iov.PushBack(p1, 1, true).IsSuccess());

	char *p2 = new char[10];
	int i;
	for(i = 0; i < 10; i++)
		p2[i] = i;
	EXPECT_TRUE(iov.PushBack(p2, 10, true).IsSuccess());

	char *p3 = new char[5];
	for(i = 0; i < 5; i++)
		p3[i] = i * 5;
	EXPECT_TRUE(iov.PushBack(p3, 5, true).IsSuccess());

	char *p4 = new char[1];
	*p4 = 0;
	EXPECT_TRUE(iov.PushBack(p4, 1, true).IsSuccess());

	char *p5 = new char[6];
	for(i = 0; i < 6; i++)
		p5[i] = i;
	EXPECT_TRUE(iov.PushBack(p5, 6, true).IsSuccess());

	CLIteratorForIOVectors iter3;
	iov.GetIterator(0, iter3);
	EXPECT_TRUE(iter3.Add(0).IsSuccess());
	
	EXPECT_TRUE(iter3.Add(1).IsSuccess());
	EXPECT_TRUE(CheckCLIOVectorIterator(p2, iter3));

	EXPECT_TRUE(iter3.Add(4).IsSuccess());
	EXPECT_TRUE(CheckCLIOVectorIterator(p2+4, iter3));

	EXPECT_TRUE(iter3.Add(11).IsSuccess());
	EXPECT_TRUE(CheckCLIOVectorIterator(p4, iter3));

	CLIteratorForIOVectors iter6(iter3);

	EXPECT_TRUE(iter3.Add(6).IsSuccess());
	EXPECT_TRUE(CheckCLIOVectorIterator(p5+5, iter3));

	CLStatus s3 = iter3.Add(1);
	EXPECT_FALSE(s3.IsSuccess());
	EXPECT_EQ(s3.m_clErrorCode, IOVECTOR_ITERATOR_END);
	EXPECT_TRUE(iter3.IsEnd());
	EXPECT_TRUE(CheckCLIOVectorIterator(0, iter3));

	CLIteratorForIOVectors iter4;
	iov.GetIterator(0, iter4);
	EXPECT_TRUE(iter4.Add(20).IsSuccess());
	EXPECT_TRUE(CheckCLIOVectorIterator(p5+3, iter4));

	EXPECT_TRUE(iter4.Sub(0).IsSuccess());

	EXPECT_TRUE(iter4.Sub(3).IsSuccess());
	EXPECT_TRUE(CheckCLIOVectorIterator(p5, iter4));

	EXPECT_TRUE(iter4.Sub(9).IsSuccess());
	EXPECT_TRUE(CheckCLIOVectorIterator(p2+7, iter4));

	CLIteratorForIOVectors iter5;
	iter5 = iter4;

	EXPECT_TRUE(iter4.Sub(4).IsSuccess());
	EXPECT_TRUE(CheckCLIOVectorIterator(p2+3, iter4));

	CLStatus s4 = iter4.Sub(5);
	EXPECT_FALSE(s4.IsSuccess());
	EXPECT_EQ(s4.m_clErrorCode, IOVECTOR_ITERATOR_END);
	EXPECT_TRUE(iter4.IsEnd());
	EXPECT_TRUE(CheckCLIOVectorIterator(0, iter4));

	EXPECT_TRUE(CheckCLIOVectorIterator(p2+7, iter5));
	EXPECT_TRUE(CheckCLIOVectorIterator(p4, iter6));
}	