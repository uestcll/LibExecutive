#include <gtest/gtest.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include "LibExecutive.h"

TEST(CLSocket, GetSocket)
{
	CLLogger::WriteLogMsg("CLSocket Test", 0);

	CLSocket s("3600");

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
	CLSocket s("3600", true);
	int fd = s.GetSocket();
	EXPECT_TRUE(fd != -1);

	int optval = 10;
	socklen_t len = 4;
	EXPECT_TRUE(getsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, &len) != -1);
	EXPECT_TRUE(optval == 1);
	EXPECT_TRUE(len == 4);

	int val = fcntl(fd, F_GETFL);
	EXPECT_TRUE(val != -1);

	EXPECT_FALSE(val & O_NONBLOCK);
}

TEST(CLSocket, TCPServerBlockV4)
{
	CLSocket s("3600", true);

	CLEvent event("test_for_socket_tcpserver");

	CLProcess *process = new CLProcess(new CLProcessFunctionForExec);
	EXPECT_TRUE(process->Run((void *)"../test_for_exec/test_for_CLSocketTCPClient/main 1").IsSuccess());

	CLStatus r3 = s.Accept(0);
	EXPECT_TRUE(r3.m_clReturnCode == -1);
	EXPECT_TRUE(r3.m_clErrorCode == NORMAL_ERROR);

	CLSocket *psocket = 0;

	CLSocket s1("127.0.0.1", "3600");
	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	CLStatus r4 = s1.Accept(&psocket);
	EXPECT_TRUE(r4.m_clReturnCode == -1);
	EXPECT_TRUE(r4.m_clErrorCode == NORMAL_ERROR);

	EXPECT_TRUE(s.Accept(&psocket).IsSuccess());

	EXPECT_TRUE(psocket != 0);

	char *pbuf = "nihaookok";
	CLIOVectors iov;

	CLStatus r5 = psocket->Write(iov);
	EXPECT_TRUE(r5.m_clReturnCode == -1);
	EXPECT_TRUE(r5.m_clErrorCode == NORMAL_ERROR);

	EXPECT_TRUE(iov.PushBack(pbuf, 9).IsSuccess());

	CLStatus r = psocket->Write(iov);
	EXPECT_TRUE(r.m_clReturnCode == 9);

	char pbuf2[10];
	bzero(pbuf2, 10);
	CLIOVectors iov2;

	CLStatus r6 = psocket->Read(iov2);
	EXPECT_TRUE(r6.m_clReturnCode == -1);
	EXPECT_TRUE(r6.m_clErrorCode == NORMAL_ERROR);

	EXPECT_TRUE(iov2.PushBack(pbuf2, 10).IsSuccess());
	CLStatus r2 = psocket->Read(iov2);
	EXPECT_TRUE(r2.m_clReturnCode == 9);

	EXPECT_TRUE(strcmp(pbuf2, pbuf) == 0);

	delete psocket;

	EXPECT_TRUE(event.Wait().IsSuccess());
}

TEST(CLSocket, TCPServerNonBlockV4)
{
	CLSocket s("3600");

	CLSocket *psocket = 0;
	CLStatus r = s.Accept(&psocket);
	EXPECT_TRUE(r.m_clReturnCode == -1);
	EXPECT_TRUE(r.m_clErrorCode == ACCEPT_PENDING);

	CLEvent event("test_for_socket_tcpserver");

	CLProcess *process = new CLProcess(new CLProcessFunctionForExec);
	EXPECT_TRUE(process->Run((void *)"../test_for_exec/test_for_CLSocketTCPClient/main 1").IsSuccess());

	while(true)
	{
		if(s.Accept(&psocket).IsSuccess() == true)
			break;
	}

	EXPECT_TRUE(psocket != 0);

	char *pbuf = "nihaookok";
	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack(pbuf, 9).IsSuccess());

	CLStatus r3 = psocket->Read(iov);
	EXPECT_TRUE(r3.m_clReturnCode == -1);
	EXPECT_TRUE(r3.m_clErrorCode == IO_PENDING);

	CLStatus r1 = psocket->Write(iov);
	EXPECT_TRUE(r1.m_clReturnCode == 9);

	char pbuf2[10];
	bzero(pbuf2, 10);
	CLIOVectors iov2;
	EXPECT_TRUE(iov2.PushBack(pbuf2, 10).IsSuccess());

	while(true)
	{
		CLStatus r2 = psocket->Read(iov2);
		if(r2.m_clReturnCode == 9)
			break;
	}

	EXPECT_TRUE(strcmp(pbuf2, pbuf) == 0);

	delete psocket;

	EXPECT_TRUE(event.Wait().IsSuccess());
}

TEST(CLSocket, TCPServerBlockV4_Error)
{
	CLSocket s("3600", true);

	CLEvent event("test_for_socket_tcpserver");
	CLEvent event2("test_for_socket_tcpserver_accept_error");

	CLProcess *process = new CLProcess(new CLProcessFunctionForExec);
	EXPECT_TRUE(process->Run((void *)"../test_for_exec/test_for_CLSocketTCPClient/main 2").IsSuccess());

	EXPECT_TRUE(event2.Wait().IsSuccess());

	CLSocket *psocket = 0;
	EXPECT_TRUE(s.Accept(&psocket).IsSuccess());
	EXPECT_TRUE(psocket != 0);

	char *pbuf = "nihaookok";
	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack(pbuf, 9).IsSuccess());

	CLStatus r1 = psocket->Read(iov);
	EXPECT_TRUE(r1.m_clReturnCode == 0);

	CLStatus r2 = psocket->Write(iov);
	EXPECT_TRUE(r2.m_clReturnCode == 9);

	sleep(3);

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);

	CLStatus r3 = psocket->Write(iov);
	EXPECT_TRUE(r3.m_clReturnCode == -1);
	EXPECT_TRUE(r3.m_clErrorCode == NORMAL_ERROR);

	delete psocket;

	EXPECT_TRUE(event.Wait().IsSuccess());
}

TEST(CLSocket, TCPClientBlockV4)
{
	CLSocket s("127.0.0.1", "3600", true);

	CLEvent event("test_for_socket_tcpserver");
	CLEvent event2("test_for_socket_tcpclient_notify");

	CLProcess *process = new CLProcess(new CLProcessFunctionForExec);
	EXPECT_TRUE(process->Run((void *)"../test_for_exec/test_for_CLSocketTCPClient/main 3").IsSuccess());

	EXPECT_TRUE(event2.Wait().IsSuccess());

	//............................
}