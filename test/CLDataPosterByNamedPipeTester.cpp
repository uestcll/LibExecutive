#include <gtest/gtest.h>
#include <string.h>
#include "LibExecutive.h"

TEST(CLDataPosterByNamedPipe, PostData_Features_Test)
{
	CLLogger::WriteLogMsg("CLDataPosterByNamedPipe Test", 0);

	CLNamedPipe qr("/tmp/NamedPipe_For_CLDataPosterByNamedPipe_Test", true);
	CLNamedPipe qw("/tmp/NamedPipe_For_CLDataPosterByNamedPipe_Test", false);

	CLDataPosterByNamedPipe dp(&qw);

	CLStatus s1 = dp.PostData(0);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, DATA_POSTER_POST_ERROR);

	CLIOVectors iov;
	CLStatus s2 = dp.PostData(&iov);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, DATA_POSTER_POST_ERROR);

	long i = 34;
	EXPECT_TRUE(iov.PushBack((char *)(&i), 8).IsSuccess());
	CLStatus s3 = dp.PostData(&iov);
	EXPECT_TRUE(s3.IsSuccess());
	EXPECT_EQ(s3.m_clReturnCode, 8);

	long j = 0;
	CLIOVectors iov1;
	EXPECT_TRUE(iov1.PushBack((char *)(&j), 8).IsSuccess());
	EXPECT_TRUE(qr.Read(iov1).IsSuccess());
	EXPECT_EQ(j, 34);
}

TEST(CLDataPosterByNamedPipe, PostData2_Features_Test)
{
	CLNamedPipe qr("/tmp/NamedPipe_For_CLDataPosterByNamedPipe_Test", true);
	CLNamedPipe qw("/tmp/NamedPipe_For_CLDataPosterByNamedPipe_Test", false);

	CLDataPosterByNamedPipe dp(&qw);

	char *pbuf = new char[64 * 1024];
	memset(pbuf, 0, 64 * 1024);
	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack(pbuf, 64*1024, true).IsSuccess());
	EXPECT_TRUE(qw.Write(iov).IsSuccess());

	CLIOVectors iov1;
	long i = 34;
	EXPECT_TRUE(iov1.PushBack((char *)(&i), 8).IsSuccess());
	CLStatus s = dp.PostData(&iov1);
	EXPECT_FALSE(s.IsSuccess());
	EXPECT_EQ(s.m_clErrorCode, DATA_POSTER_POST_PENDING);
}