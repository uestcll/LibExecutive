#include <gtest/gtest.h>
#include <string.h>
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

TEST(CLIOVectors, Clear_Features_Test)
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

	iovs.Clear();

	EXPECT_TRUE(CheckIOVectorStatus(0, 0, iovs));
}

TEST(CLIOVectors, GetIndex_Features_Test)
{
	CLIOVectors iov1, iov2;
	CLIteratorForIOVectors iter1, iter2;
	unsigned int index;
	
	iov1.GetIterator(0, iter1);
	CLStatus s1 = iov1.GetIndex(iter1, index);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, NORMAL_ERROR);

	char *p1 = new char[1];
	*p1 = 2;

	char *p2 = new char[10];
	int i;
	for(i = 0; i < 10; i++)
		p2[i] = i;

	char *p3 = new char[5];
	for(i = 0; i < 5; i++)
		p3[i] = i * 5;

	char *p4 = new char[1];
	*p4 = 0;

	char *p5 = new char[6];
	for(i = 0; i < 6; i++)
		p5[i] = i;

	EXPECT_TRUE(iov2.PushBack(p1, 1).IsSuccess());
	iov2.GetIterator(0, iter2);

	EXPECT_TRUE(iov1.PushBack(p1, 1, true).IsSuccess());
	EXPECT_TRUE(iov1.PushBack(p2, 10, true).IsSuccess());
	EXPECT_TRUE(iov1.PushBack(p3, 5, true).IsSuccess());
	EXPECT_TRUE(iov1.PushBack(p4, 1, true).IsSuccess());
	EXPECT_TRUE(iov1.PushBack(p5, 6, true).IsSuccess());

	CLStatus s2 = iov1.GetIndex(iter2, index);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, NORMAL_ERROR);

	iov1.GetIterator(0, iter1);
	EXPECT_TRUE(iov1.GetIndex(iter1, index).IsSuccess());
	EXPECT_EQ(index, 0);

	EXPECT_TRUE(iter1.Add(4).IsSuccess());
	EXPECT_TRUE(iov1.GetIndex(iter1, index).IsSuccess());
	EXPECT_EQ(index, 4);

	EXPECT_TRUE(iter1.Add(12).IsSuccess());
	EXPECT_TRUE(iov1.GetIndex(iter1, index).IsSuccess());
	EXPECT_EQ(index, 16);

	EXPECT_TRUE(iter1.Add(1).IsSuccess());
	EXPECT_TRUE(iov1.GetIndex(iter1, index).IsSuccess());
	EXPECT_EQ(index, 17);

	EXPECT_TRUE(iter1.Add(5).IsSuccess());
	EXPECT_TRUE(iov1.GetIndex(iter1, index).IsSuccess());
	EXPECT_EQ(index, 22);
}

TEST(CLIOVectors, WriteBlockIterator_Features_Test)
{
	CLIOVectors iov;
	CLIteratorForIOVectors iter;
	
	CLStatus s1 = iov.WriteBlock(iter, 0, 0);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, NORMAL_ERROR);

	CLStatus s2 = iov.WriteBlock(iter, (char *)2, 0);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, NORMAL_ERROR);

	CLStatus s3 = iov.WriteBlock(iter, (char *)2, 2);
	EXPECT_FALSE(s3.IsSuccess());
	EXPECT_EQ(s3.m_clErrorCode, NORMAL_ERROR);

	char *p1 = new char[1];
	char *p2 = new char[10];
	char *p3 = new char[5];
	char *p4 = new char[1];
	char *p5 = new char[6];

	EXPECT_TRUE(iov.PushBack(p1, 1, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p2, 10, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p3, 5, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p4, 1, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p5, 6, true).IsSuccess());

	iov.GetIterator(0, iter);
	char x = 2;
	CLStatus s4 = iov.WriteBlock(iter, &x, 1);
	EXPECT_EQ(s4.m_clReturnCode, 1);
	unsigned int index;
	EXPECT_TRUE(iov.GetIndex(iter, index).IsSuccess());
	EXPECT_EQ(index, 1);

	char buf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	CLStatus s5 = iov.WriteBlock(iter, buf, 3);
	EXPECT_EQ(s5.m_clReturnCode, 3);
	EXPECT_TRUE(iov.GetIndex(iter, index).IsSuccess());
	EXPECT_EQ(index, 4);

	CLStatus s6 = iov.WriteBlock(iter, buf+3, 4);
	EXPECT_EQ(s6.m_clReturnCode, 4);
	EXPECT_TRUE(iov.GetIndex(iter, index).IsSuccess());
	EXPECT_EQ(index, 8);

	CLStatus s7 = iov.WriteBlock(iter, buf+7, 3);
	EXPECT_EQ(s7.m_clReturnCode, 3);
	EXPECT_TRUE(iov.GetIndex(iter, index).IsSuccess());
	EXPECT_EQ(index, 11);

	char buf1[10] = {0, 5, 10, 15, 20};
	CLStatus s8 = iov.WriteBlock(iter, buf1, 5);
	EXPECT_EQ(s8.m_clReturnCode, 5);
	EXPECT_TRUE(iov.GetIndex(iter, index).IsSuccess());
	EXPECT_EQ(index, 16);

	char buf2[16] = {3, 4, 5, 6, 7, 8, 9, 0, 5, 10, 15, 20, 0, 0, 1, 2};
	iov.GetIterator(4, iter);
	CLStatus s9 = iov.WriteBlock(iter, buf2, 16);
	EXPECT_EQ(s9.m_clReturnCode, 16);
	EXPECT_TRUE(iov.GetIndex(iter, index).IsSuccess());
	EXPECT_EQ(index, 20);

	char buf3[3] = {3, 4, 5};
	CLStatus s10 = iov.WriteBlock(iter, buf3, 3);
	EXPECT_EQ(s10.m_clReturnCode, 3);
	EXPECT_TRUE(iter.IsEnd());

	iov.GetIterator(13, iter);
	char buf4[11] = {10, 15, 20, 0, 0, 1, 2, 3, 4, 5, 6};
	CLStatus s11 = iov.WriteBlock(iter, buf4, 11);
	EXPECT_EQ(s11.m_clReturnCode, 10);
	EXPECT_TRUE(iter.IsEnd());

	iov.GetIterator(13, iter);
	char buf5[10] = {10, 15, 20, 0, 0, 1, 2, 3, 4, 5};
	CLStatus s12 = iov.WriteBlock(iter, buf5, 10);
	EXPECT_EQ(s12.m_clReturnCode, 10);
	EXPECT_TRUE(iter.IsEnd());

	char buf6[13] = {3, 4, 5, 6, 7, 8, 9, 0, 5, 10, 15, 20, 0};
	iov.GetIterator(4, iter);
	CLStatus s13 = iov.WriteBlock(iter, buf6, 13);
	EXPECT_EQ(s13.m_clReturnCode, 13);
	EXPECT_TRUE(iov.GetIndex(iter, index).IsSuccess());
	EXPECT_EQ(index, 17);

	char buf7[23] = {2, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 5, 10, 15, 20, 0, 0, 1, 2, 3, 4, 5};
	EXPECT_TRUE(CheckIOVectorData(buf7, 23, iov));
}

TEST(CLIOVectors, ReadBlockIterator_Features_Test)
{
	CLIOVectors iov;

	char *p1 = new char[1];
	*p1 = 2;

	char *p2 = new char[10];
	int i;
	for(i = 0; i < 10; i++)
		p2[i] = i;

	char *p3 = new char[5];
	for(i = 0; i < 5; i++)
		p3[i] = i * 5;

	char *p4 = new char[1];
	*p4 = 0;

	char *p5 = new char[6];
	for(i = 0; i < 6; i++)
		p5[i] = i;

	EXPECT_TRUE(iov.PushBack(p1, 1, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p2, 10, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p3, 5, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p4, 1, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p5, 6, true).IsSuccess());

	CLIteratorForIOVectors iter;

	CLStatus s1 = iov.ReadBlock(iter, 0, 0);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, NORMAL_ERROR);

	CLStatus s2 = iov.ReadBlock(iter, (char *)2, 0);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, NORMAL_ERROR);

	CLStatus s3 = iov.ReadBlock(iter, (char *)2, 2);
	EXPECT_FALSE(s3.IsSuccess());
	EXPECT_EQ(s3.m_clErrorCode, NORMAL_ERROR);

	iov.GetIterator(0, iter);
	char x;
	CLStatus s4 = iov.ReadBlock(iter, &x, 1);
	EXPECT_EQ(s4.m_clReturnCode, 1);
	unsigned int index;
	EXPECT_TRUE(iov.GetIndex(iter, index).IsSuccess());
	EXPECT_EQ(index, 1);
	EXPECT_EQ(x, 2);

	char buf[23] = {2, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 5, 10, 15, 20, 0, 0, 1, 2, 3, 4, 5};

	char buffer[30];
	CLStatus s5 = iov.ReadBlock(iter, buffer, 3);
	EXPECT_EQ(s5.m_clReturnCode, 3);
	EXPECT_TRUE(iov.GetIndex(iter, index).IsSuccess());
	EXPECT_EQ(index, 4);
	EXPECT_EQ(memcmp(buffer, buf + 1, 3), 0);

	CLStatus s6 = iov.ReadBlock(iter, buffer, 4);
	EXPECT_EQ(s6.m_clReturnCode, 4);
	EXPECT_TRUE(iov.GetIndex(iter, index).IsSuccess());
	EXPECT_EQ(index, 8);
	EXPECT_EQ(memcmp(buffer, buf + 4, 4), 0);

	CLStatus s7 = iov.ReadBlock(iter, buffer, 3);
	EXPECT_EQ(s7.m_clReturnCode, 3);
	EXPECT_TRUE(iov.GetIndex(iter, index).IsSuccess());
	EXPECT_EQ(index, 11);
	EXPECT_EQ(memcmp(buffer, buf + 8, 3), 0);

	CLStatus s8 = iov.ReadBlock(iter, buffer, 5);
	EXPECT_EQ(s8.m_clReturnCode, 5);
	EXPECT_TRUE(iov.GetIndex(iter, index).IsSuccess());
	EXPECT_EQ(index, 16);
	EXPECT_EQ(memcmp(buffer, buf + 11, 5), 0);

	iov.GetIterator(4, iter);
	CLStatus s9 = iov.ReadBlock(iter, buffer, 16);
	EXPECT_EQ(s9.m_clReturnCode, 16);
	EXPECT_TRUE(iov.GetIndex(iter, index).IsSuccess());
	EXPECT_EQ(index, 20);
	EXPECT_EQ(memcmp(buffer, buf + 4, 16), 0);

	CLStatus s10 = iov.ReadBlock(iter, buffer, 3);
	EXPECT_EQ(s10.m_clReturnCode, 3);
	EXPECT_TRUE(iter.IsEnd());
	EXPECT_EQ(memcmp(buffer, buf + 20, 3), 0);

	iov.GetIterator(13, iter);
	CLStatus s11 = iov.ReadBlock(iter, buffer, 11);
	EXPECT_EQ(s11.m_clReturnCode, 10);
	EXPECT_TRUE(iter.IsEnd());
	EXPECT_EQ(memcmp(buffer, buf + 13, 10), 0);

	iov.GetIterator(13, iter);
	CLStatus s12 = iov.ReadBlock(iter, buffer, 10);
	EXPECT_EQ(s12.m_clReturnCode, 10);
	EXPECT_TRUE(iter.IsEnd());
	EXPECT_EQ(memcmp(buffer, buf + 13, 10), 0);

	iov.GetIterator(4, iter);
	CLStatus s13 = iov.ReadBlock(iter, buffer, 13);
	EXPECT_EQ(s13.m_clReturnCode, 13);
	EXPECT_TRUE(iov.GetIndex(iter, index).IsSuccess());
	EXPECT_EQ(index, 17);
	EXPECT_EQ(memcmp(buffer, buf + 4, 13), 0);
}

TEST(CLIOVectors, WriteBlockIndex_Features_Test)
{
	CLIOVectors iov;
	CLStatus s1 = iov.WriteBlock(0, 0, 2);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, NORMAL_ERROR);

	CLStatus s2 = iov.WriteBlock(0, (char *)2, 0);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, NORMAL_ERROR);

	CLStatus s3 = iov.WriteBlock(2, (char *)2, 2);
	EXPECT_FALSE(s3.IsSuccess());
	EXPECT_EQ(s3.m_clErrorCode, NORMAL_ERROR);

	char *p1 = new char[1];
	memset(p1, 0, 1);
	char *p2 = new char[10];
	memset(p2, 0, 10);
	char *p3 = new char[5];
	memset(p3, 0, 5);
	char *p4 = new char[1];
	memset(p4, 0, 1);
	char *p5 = new char[6];
	memset(p5, 0, 6);

	EXPECT_TRUE(iov.PushBack(p1, 1, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p2, 10, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p3, 5, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p4, 1, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p5, 6, true).IsSuccess());

	CLStatus s0 = iov.WriteBlock(23, (char *)2, 2);
	EXPECT_FALSE(s0.IsSuccess());
	EXPECT_EQ(s0.m_clErrorCode, NORMAL_ERROR);

	char total[23];
	memset(total, 0, 23);

	char x = 2;
	total[0] = 2;
	CLStatus s4 = iov.WriteBlock(0, &x, 1);
	EXPECT_EQ(s4.m_clReturnCode, 1);
	EXPECT_TRUE(CheckIOVectorData(total, 23, iov));

	char buf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	CLStatus s5 = iov.WriteBlock(1, buf, 3);
	EXPECT_EQ(s5.m_clReturnCode, 3);
	total[1] = 0;
	total[2] = 1;
	total[3] = 2;
	EXPECT_TRUE(CheckIOVectorData(total, 23, iov));

	CLStatus s6 = iov.WriteBlock(4, buf+3, 4);
	EXPECT_EQ(s6.m_clReturnCode, 4);
	total[4] = 3;
	total[5] = 4;
	total[6] = 5;
	total[7] = 6;
	EXPECT_TRUE(CheckIOVectorData(total, 23, iov));

	CLStatus s7 = iov.WriteBlock(8, buf+7, 3);
	EXPECT_EQ(s7.m_clReturnCode, 3);
	total[8] = 7;
	total[9] = 8;
	total[10] = 9;
	EXPECT_TRUE(CheckIOVectorData(total, 23, iov));

	char buf1[10] = {0, 5, 10, 15, 20};
	CLStatus s8 = iov.WriteBlock(11, buf1, 5);
	EXPECT_EQ(s8.m_clReturnCode, 5);
	total[11] = 0;
	total[12] = 5;
	total[13] = 10;
	total[14] = 15;
	total[15] = 20;
	EXPECT_TRUE(CheckIOVectorData(total, 23, iov));

	char buf2[16] = {3, 4, 5, 6, 7, 8, 9, 0, 5, 10, 15, 20, 0, 0, 1, 2};
	CLStatus s9 = iov.WriteBlock(4, buf2, 16);
	EXPECT_EQ(s9.m_clReturnCode, 16);
	total[16] = 0;
	total[17] = 0;
	total[18] = 1;
	total[19] = 2;
	EXPECT_TRUE(CheckIOVectorData(total, 23, iov));

	char buf3[3] = {3, 4, 5};
	CLStatus s10 = iov.WriteBlock(20, buf3, 3);
	EXPECT_EQ(s10.m_clReturnCode, 3);
	total[20] = 3;
	total[21] = 4;
	total[22] = 5;
	EXPECT_TRUE(CheckIOVectorData(total, 23, iov));

	char buf4[11] = {10, 15, 20, 0, 0, 1, 2, 3, 4, 5, 6};
	CLStatus s11 = iov.WriteBlock(13, buf4, 11);
	EXPECT_EQ(s11.m_clReturnCode, 10);
	EXPECT_TRUE(CheckIOVectorData(total, 23, iov));

	char buf5[10] = {10, 15, 20, 0, 0, 1, 2, 3, 4, 5};
	CLStatus s12 = iov.WriteBlock(13, buf5, 10);
	EXPECT_EQ(s12.m_clReturnCode, 10);
	EXPECT_TRUE(CheckIOVectorData(total, 23, iov));

	char buf6[13] = {3, 4, 5, 6, 7, 8, 9, 0, 5, 10, 15, 20, 0};
	CLStatus s13 = iov.WriteBlock(4, buf6, 13);
	EXPECT_EQ(s13.m_clReturnCode, 13);
	EXPECT_TRUE(CheckIOVectorData(total, 23, iov));

	char buf7[23] = {2, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 5, 10, 15, 20, 0, 0, 1, 2, 3, 4, 5};
	EXPECT_TRUE(CheckIOVectorData(buf7, 23, iov));
	EXPECT_TRUE(CheckIOVectorData(total, 23, iov));
}

TEST(CLIOVectors, ReadBlockIndex_Features_Test)
{
	CLIOVectors iov;
	
	CLStatus s1 = iov.ReadBlock(0, 0, 2);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, NORMAL_ERROR);

	CLStatus s2 = iov.ReadBlock(0, (char *)2, 0);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, NORMAL_ERROR);

	CLStatus s3 = iov.ReadBlock(0, (char *)2, 2);
	EXPECT_FALSE(s3.IsSuccess());
	EXPECT_EQ(s3.m_clErrorCode, NORMAL_ERROR);

	char *p1 = new char[1];
	*p1 = 2;

	char *p2 = new char[10];
	int i;
	for(i = 0; i < 10; i++)
		p2[i] = i;

	char *p3 = new char[5];
	for(i = 0; i < 5; i++)
		p3[i] = i * 5;

	char *p4 = new char[1];
	*p4 = 0;

	char *p5 = new char[6];
	for(i = 0; i < 6; i++)
		p5[i] = i;

	EXPECT_TRUE(iov.PushBack(p1, 1, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p2, 10, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p3, 5, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p4, 1, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p5, 6, true).IsSuccess());

	char x;
	CLStatus s4 = iov.ReadBlock(0, &x, 1);
	EXPECT_EQ(s4.m_clReturnCode, 1);
	EXPECT_EQ(x, 2);

	char buf[23] = {2, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 5, 10, 15, 20, 0, 0, 1, 2, 3, 4, 5};

	char buffer[30];
	CLStatus s5 = iov.ReadBlock(1, buffer, 3);
	EXPECT_EQ(s5.m_clReturnCode, 3);
	EXPECT_EQ(memcmp(buffer, buf + 1, 3), 0);

	CLStatus s6 = iov.ReadBlock(4, buffer, 4);
	EXPECT_EQ(s6.m_clReturnCode, 4);
	EXPECT_EQ(memcmp(buffer, buf + 4, 4), 0);

	CLStatus s7 = iov.ReadBlock(8, buffer, 3);
	EXPECT_EQ(s7.m_clReturnCode, 3);
	EXPECT_EQ(memcmp(buffer, buf + 8, 3), 0);

	CLStatus s8 = iov.ReadBlock(11, buffer, 5);
	EXPECT_EQ(s8.m_clReturnCode, 5);
	EXPECT_EQ(memcmp(buffer, buf + 11, 5), 0);

	CLStatus s9 = iov.ReadBlock(4, buffer, 16);
	EXPECT_EQ(s9.m_clReturnCode, 16);
	EXPECT_EQ(memcmp(buffer, buf + 4, 16), 0);

	CLStatus s10 = iov.ReadBlock(20, buffer, 3);
	EXPECT_EQ(s10.m_clReturnCode, 3);
	EXPECT_EQ(memcmp(buffer, buf + 20, 3), 0);

	CLStatus s11 = iov.ReadBlock(13, buffer, 11);
	EXPECT_EQ(s11.m_clReturnCode, 10);
	EXPECT_EQ(memcmp(buffer, buf + 13, 10), 0);

	CLStatus s12 = iov.ReadBlock(13, buffer, 10);
	EXPECT_EQ(s12.m_clReturnCode, 10);
	EXPECT_EQ(memcmp(buffer, buf + 13, 10), 0);

	CLStatus s13 = iov.ReadBlock(4, buffer, 13);
	EXPECT_EQ(s13.m_clReturnCode, 13);
	EXPECT_EQ(memcmp(buffer, buf + 4, 13), 0);
}

TEST(CLIOVectors, PushBackRangeToAIOVectorIter_Features_Test)
{
	CLIOVectors iov;
	CLIOVectors tio;
	CLIteratorForIOVectors iter;
	
	iov.GetIterator(0, iter);
	CLStatus s1 = iov.PushBackRangeToAIOVector(tio, iter, 23);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, NORMAL_ERROR);

	char *p1 = new char[1];
	*p1 = 2;

	char *p2 = new char[10];
	int i;
	for(i = 0; i < 10; i++)
		p2[i] = i;

	char *p3 = new char[5];
	for(i = 0; i < 5; i++)
		p3[i] = i * 5;

	char *p4 = new char[1];
	*p4 = 0;

	char *p5 = new char[6];
	for(i = 0; i < 6; i++)
		p5[i] = i;

	EXPECT_TRUE(iov.PushBack(p1, 1, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p2, 10, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p3, 5, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p4, 1, true).IsSuccess());
	EXPECT_TRUE(iov.PushBack(p5, 6, true).IsSuccess());

	iov.GetIterator(1, iter);
	CLStatus s2 = iov.PushBackRangeToAIOVector(tio, iter, 0);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, NORMAL_ERROR);

	CLStatus s3 = iov.PushBackRangeToAIOVector(tio, iter, 4, 5);
	EXPECT_FALSE(s3.IsSuccess());
	EXPECT_EQ(s3.m_clErrorCode, NORMAL_ERROR);
}