#include <gtest/gtest.h>
#include <fcntl.h>
#include "LibExecutive.h"

TEST(CLDataReceiverByNamedPipe, GetData_Features_Test)
{
	CLLogger::WriteLogMsg("CLDataReceiverByNamedPipe Test", 0);

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
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, NORMAL_ERROR);
}

TEST(CLDataReceiverByNamedPipe, GetData2_Features_Test)
{
	CLDataReceiverByNamedPipe dr("/tmp/CLNamedPipe_Read_Features_Test");

	CLIOVectors iov;
	char *p = new char[20];
	EXPECT_TRUE(iov.PushBack(p, 20, true).IsSuccess());
	long context = 0;
	CLStatus s1 = dr.GetData(iov, &context);
	EXPECT_EQ(s1.m_clErrorCode, RECEIVED_ZERO_BYTE);
	EXPECT_NE(context, 0);
}

TEST(CLDataReceiverByNamedPipe, GetUuid)
{
	CLDataReceiverByNamedPipe s1("/tmp/CLNamedPipe_GetUuid1_Features_Test");
	CLDataReceiverByNamedPipe s2("/tmp/CLNamedPipe_GetUuid2_Features_Test");

	CLUuid u1 = s1.GetUuid();
	CLUuid u2 = s2.GetUuid();

	void *p1 = (char *)&u1 + sizeof(long);
	void *p2 = (char *)&u2 + sizeof(long);

	int r = uuid_compare((unsigned char *)p1, (unsigned char *)p2);
	EXPECT_TRUE(r != 0);
}