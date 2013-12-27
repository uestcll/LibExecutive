#include <gtest/gtest.h>
#include <fcntl.h>
#include "LibExecutive.h"

TEST(CLDataReceiverByNamedPipe, GetData_Features_Test)
{
	CLDataReceiverByNamedPipe dr("/tmp/CLNamedPipe_Read_Features_Test");

	int fd = open("/tmp/CLNamedPipe_Read_Features_Test", O_WRONLY);
	char buf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	write(fd, buf, 10);
	close(fd);

	CLIOVectors iov;
	char *p = new char[20];
	EXPECT_TRUE(iov.PushBack(p, 20, true).IsSuccess());
	long context = 0;
	CLStatus s1 = dr.GetData(iov, &context);
	EXPECT_EQ(s1.m_clReturnCode, 10);
	EXPECT_EQ(memcmp(p, buf, 10), 0);
	EXPECT_NE(context, 0);
	context = 0;

	CLStatus s3 = dr.GetData(iov, &context);
	EXPECT_NE(context, 0);
	context = 0;
	EXPECT_FALSE(s3.IsSuccess());
	EXPECT_EQ(s3.m_clErrorCode, RECEIVED_ZERO_BYTE);


	iov.Clear();
	CLStatus s2 = dr.GetData(iov, &context);
	EXPECT_NE(context, 0);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, NORMAL_ERROR);
}