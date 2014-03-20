#include <gtest/gtest.h>
#include <memory.h>
#include <fcntl.h>
#include "LibExecutive.h"

TEST(CLNamedPipe, Read_Features_Test)
{
	CLLogger::WriteLogMsg("CLNamedPipe Test", 0);

	CLNamedPipe pipe("/tmp/CLNamedPipe_Read_Features_Test", true);

	EXPECT_EQ(pipe.GetSizeForAtomWriting(), 4096);

	int fd = open("/tmp/CLNamedPipe_Read_Features_Test", O_WRONLY);
	char buf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	write(fd, buf, 10);
	close(fd);

	CLIOVectors iov;
	char *p = new char[20];
	EXPECT_TRUE(iov.PushBack(p, 20, true).IsSuccess());
	CLStatus s1 = pipe.Read(iov);
	EXPECT_EQ(s1.m_clReturnCode, 10);
	EXPECT_EQ(memcmp(p, buf, 10), 0);

	CLStatus s2 = pipe.Write(iov);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, NORMAL_ERROR);

	iov.Clear();
	CLStatus s3 = pipe.Read(iov);
	EXPECT_FALSE(s3.IsSuccess());
	EXPECT_EQ(s3.m_clErrorCode, NORMAL_ERROR);
}

TEST(CLNamedPipe, Read_Features2_Test)
{
	CLNamedPipe pipe("/tmp/CLNamedPipe_Read_Features_Test", true);

	int fd = open("/tmp/CLNamedPipe_Read_Features_Test", O_WRONLY);
	char buf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	write(fd, buf, 10);
	close(fd);

	CLIOVectors iov;
	char *p = new char[3];
	EXPECT_TRUE(iov.PushBack(p, 3, true).IsSuccess());
	CLStatus s1 = pipe.Read(iov);
	EXPECT_EQ(s1.m_clReturnCode, 3);
	EXPECT_EQ(memcmp(p, buf, 3), 0);

	CLStatus s2 = pipe.Read(iov);
	EXPECT_EQ(s2.m_clReturnCode, 3);
	EXPECT_EQ(memcmp(p, buf+3, 3), 0);

	CLStatus s3 = pipe.Read(iov);
	EXPECT_EQ(s3.m_clReturnCode, 3);
	EXPECT_EQ(memcmp(p, buf+6, 3), 0);

	CLStatus s4 = pipe.Read(iov);
	EXPECT_EQ(s4.m_clReturnCode, 1);
	EXPECT_EQ(memcmp(p, buf+9, 1), 0);
}

TEST(CLNamedPipe, Read_Features3_Test)
{
	CLNamedPipe pipe("/tmp/CLNamedPipe_Read_Features_Test", true, "mutex_for_CLNamedPipe_Read_Features_Test");

	EXPECT_EQ(pipe.GetSizeForAtomWriting(), 4096);

	int fd = open("/tmp/CLNamedPipe_Read_Features_Test", O_WRONLY);
	char buf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	write(fd, buf, 10);
	close(fd);

	CLIOVectors iov;
	char *p = new char[20];
	EXPECT_TRUE(iov.PushBack(p, 20, true).IsSuccess());
	CLStatus s1 = pipe.Read(iov);
	EXPECT_EQ(s1.m_clReturnCode, 10);
	EXPECT_EQ(memcmp(p, buf, 10), 0);

	CLStatus s2 = pipe.Write(iov);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, NORMAL_ERROR);

	iov.Clear();
	CLStatus s3 = pipe.Read(iov);
	EXPECT_FALSE(s3.IsSuccess());
	EXPECT_EQ(s3.m_clErrorCode, NORMAL_ERROR);
}

TEST(CLNamedPipe, Read_Features4_Test)
{
	CLNamedPipe pipe("/tmp/CLNamedPipe_Read_Features_Test", true, "mutex_for_CLNamedPipe_Read_Features_Test");

	int fd = open("/tmp/CLNamedPipe_Read_Features_Test", O_WRONLY);
	char buf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	write(fd, buf, 10);
	close(fd);

	CLIOVectors iov;
	char *p = new char[3];
	EXPECT_TRUE(iov.PushBack(p, 3, true).IsSuccess());
	CLStatus s1 = pipe.Read(iov);
	EXPECT_EQ(s1.m_clReturnCode, 3);
	EXPECT_EQ(memcmp(p, buf, 3), 0);

	CLStatus s2 = pipe.Read(iov);
	EXPECT_EQ(s2.m_clReturnCode, 3);
	EXPECT_EQ(memcmp(p, buf+3, 3), 0);

	CLStatus s3 = pipe.Read(iov);
	EXPECT_EQ(s3.m_clReturnCode, 3);
	EXPECT_EQ(memcmp(p, buf+6, 3), 0);

	CLStatus s4 = pipe.Read(iov);
	EXPECT_EQ(s4.m_clReturnCode, 1);
	EXPECT_EQ(memcmp(p, buf+9, 1), 0);
}

TEST(CLNamedPipe, Write_Features_Test)
{
	int fd = open("/tmp/CLNamedPipe_Read_Features_Test", O_RDONLY | O_NONBLOCK);
	char buf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

	CLNamedPipe pipe("/tmp/CLNamedPipe_Read_Features_Test", false);
	EXPECT_EQ(pipe.GetSizeForAtomWriting(), 4096);

	CLIOVectors iov;

	CLStatus s1 = pipe.Read(iov);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, NORMAL_ERROR);

	CLStatus s2 = pipe.Write(iov);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, NORMAL_ERROR);

	EXPECT_TRUE(iov.PushBack(buf, 10).IsSuccess());
	CLStatus s3 = pipe.Write(iov);
	EXPECT_EQ(s3.m_clReturnCode, 10);

	char buf1[10];
	read(fd, buf1, 10);
	EXPECT_EQ(memcmp(buf, buf1, 10), 0);
	
	close(fd);
}

TEST(CLNamedPipe, Write_Features2_Test)
{
	int fd = open("/tmp/CLNamedPipe_Read_Features_Test", O_RDONLY | O_NONBLOCK);
	char buf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

	CLNamedPipe pipe("/tmp/CLNamedPipe_Read_Features_Test", false, "mutex_for_CLNamedPipe_Read_Features_Test");
	EXPECT_EQ(pipe.GetSizeForAtomWriting(), 4096);

	CLIOVectors iov;

	CLStatus s1 = pipe.Read(iov);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, NORMAL_ERROR);

	CLStatus s2 = pipe.Write(iov);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, NORMAL_ERROR);

	EXPECT_TRUE(iov.PushBack(buf, 10).IsSuccess());
	CLStatus s3 = pipe.Write(iov);
	EXPECT_EQ(s3.m_clReturnCode, 10);

	char buf1[10];
	read(fd, buf1, 10);
	EXPECT_EQ(memcmp(buf, buf1, 10), 0);

	close(fd);
}

TEST(CLNamedPipe, GetUuid)
{
	CLNamedPipe s1("/tmp/CLNamedPipe_GetUuid1_Features_Test", true);
	CLNamedPipe s2("/tmp/CLNamedPipe_GetUuid2_Features_Test", true);

	CLUuid u1 = s1.GetUuid();
	CLUuid u2 = s2.GetUuid();

	void *p1 = (char *)&u1 + sizeof(long);
	void *p2 = (char *)&u2 + sizeof(long);

	int r = uuid_compare((unsigned char *)p1, (unsigned char *)p2);
	EXPECT_TRUE(r != 0);
}