#include <gtest/gtest.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include "LibExecutive.h"

TEST(CLSocket, GetSocket)
{
	CLLogger::WriteLogMsg("CLSocket Test", 0);

	CLSocket s;
	int fd = s.GetSocket();
	EXPECT_TRUE(fd != -1);

	int optval = 10;
	socklen_t len = 4;
	EXPECT_TRUE(getsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, &len) != -1);
	EXPECT_TRUE(optval == 1);
	EXPECT_TRUE(len == 4);

	int val = fcntl(fd, F_GETFL);
	EXPECT_TRUE(val != -1);

	EXPECT_TRUE(val & O_NONBLOCK);
}

TEST(CLSocket, BlockSocket)
{
	CLSocket s(true);
	int fd = s.GetSocket();
	EXPECT_TRUE(fd != -1);

	int val = fcntl(fd, F_GETFL);
	EXPECT_TRUE(val != -1);

	EXPECT_FALSE(val & O_NONBLOCK);
}