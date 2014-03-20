#include <gtest/gtest.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <netdb.h>
#include <uuid/uuid.h>
#include <string.h>
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
	CLSocket s("3600", SOCKET_TYPE_TCP, true);
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
	CLSocket s("3600", SOCKET_TYPE_TCP, true);

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

	const char *pbuf = "nihaookok";
	CLIOVectors iov;

	CLStatus r5 = psocket->Write(iov);
	EXPECT_TRUE(r5.m_clReturnCode == -1);
	EXPECT_TRUE(r5.m_clErrorCode == NORMAL_ERROR);

	EXPECT_TRUE(iov.PushBack((char *)pbuf, 9).IsSuccess());

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

	const char *pbuf = "nihaookok";
	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack((char *)pbuf, 9).IsSuccess());

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
	CLSocket s("3600", SOCKET_TYPE_TCP, true);

	CLEvent event("test_for_socket_tcpserver");
	CLEvent event2("test_for_socket_tcpserver_accept_error");

	CLProcess *process = new CLProcess(new CLProcessFunctionForExec);
	EXPECT_TRUE(process->Run((void *)"../test_for_exec/test_for_CLSocketTCPClient/main 2").IsSuccess());

	EXPECT_TRUE(event2.Wait().IsSuccess());

	CLSocket *psocket = 0;
	EXPECT_TRUE(s.Accept(&psocket).IsSuccess());
	EXPECT_TRUE(psocket != 0);

	const char *pbuf = "nihaookok";
	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack((char *)pbuf, 9).IsSuccess());

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
	{
		CLSocket s("3600", SOCKET_TYPE_TCP, true);

		CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
		CLStatus r1 = s.Connect();
		EXPECT_TRUE(r1.m_clReturnCode == -1);
		EXPECT_TRUE(r1.m_clErrorCode == NORMAL_ERROR);
	}

	CLSocket s("127.0.0.1", "3600", SOCKET_TYPE_TCP, true);

	CLEvent event("test_for_socket_tcpserver");
	CLEvent event2("test_for_socket_tcpclient_notify");

	CLProcess *process = new CLProcess(new CLProcessFunctionForExec);
	EXPECT_TRUE(process->Run((void *)"../test_for_exec/test_for_CLSocketTCPClient/main 3").IsSuccess());

	EXPECT_TRUE(event2.Wait().IsSuccess());

	EXPECT_TRUE(s.Connect().IsSuccess());

	CLStatus r1 = s.Connect();
	EXPECT_TRUE(r1.m_clReturnCode == -1);
	EXPECT_TRUE(r1.m_clErrorCode == NORMAL_ERROR);

	char pbuf[15];
	bzero(pbuf, 15);
	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack(pbuf, 15).IsSuccess());
	CLStatus r = s.Read(iov);
	EXPECT_TRUE(r.m_clReturnCode == 13);

	const char *str = "Hello World\n";
	EXPECT_TRUE(strcmp(str, pbuf) == 0);

	EXPECT_TRUE(event.Wait().IsSuccess());
}

TEST(CLSocket, TCPClientBlockV4_Error)
{
	CLSocket s("127.0.0.1", "3600", SOCKET_TYPE_TCP, true);

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	CLStatus r1 = s.Connect();
	EXPECT_TRUE(r1.m_clReturnCode == -1);
	EXPECT_TRUE(r1.m_clErrorCode == TRY_CONNECT_END);
}

TEST(CLSocket, TCPClientNonBlockV4)
{
	CLSocket s("192.157.2.4", "3600");

	CLStatus r = s.Connect();
	EXPECT_TRUE(r.m_clReturnCode == -1);
	EXPECT_TRUE(r.m_clErrorCode == CONNECT_PENDING);

	int fd = s.GetSocket();
	int maxfdp1 = fd + 1;
	fd_set rd;
	fd_set wr;
	FD_ZERO(&rd);
	FD_ZERO(&wr);
	FD_SET(fd, &rd);
	FD_SET(fd, &wr);

	int num = select(maxfdp1, &rd, &wr, 0, 0);
	EXPECT_EQ(num, 2);
	EXPECT_TRUE(FD_ISSET(fd, &rd));
	EXPECT_TRUE(FD_ISSET(fd, &wr));

	int err = 0;
	socklen_t len = 4;
	if(getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0)
	{
		err = 2;
	}
	else if(err != 0)
	{
		err = 2;
	}

	EXPECT_EQ(err, 2);

	CLStatus r1 = s.Connect();
	EXPECT_TRUE(r1.m_clReturnCode == -1);
	EXPECT_TRUE(r1.m_clErrorCode == NORMAL_ERROR);

	s.NotifyConnectResults(false);

	CLStatus r2 = s.Connect();
	EXPECT_TRUE(r2.m_clReturnCode == -1);
	EXPECT_TRUE(r2.m_clErrorCode == TRY_CONNECT_END);
}

TEST(CLSocket, UDP_GetSocket)
{
	CLSocket s("3600", SOCKET_TYPE_UDP);

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

TEST(CLSocket, UDP_BlockSocket)
{
	CLSocket s("3600", SOCKET_TYPE_UDP, true);
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

TEST(CLSocket, UDPServerBlockV4)
{
	CLSocket s("3600", SOCKET_TYPE_UDP, true);

	CLEvent event("test_for_socket_tcpserver");

	CLProcess *process = new CLProcess(new CLProcessFunctionForExec);
	EXPECT_TRUE(process->Run((void *)"../test_for_exec/test_for_CLSocketTCPClient/main 4").IsSuccess());

	char buf[10];
	bzero(buf, 10);
	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack(buf, 10).IsSuccess());

	CLSocketAddress addr;
	CLStatus r = s.Read(iov, &addr);
	EXPECT_TRUE(r.m_clReturnCode == 9);
	EXPECT_TRUE(r.m_clErrorCode == 0);

	const char *pbuf = "nihaookok";
	
	EXPECT_TRUE(strcmp(pbuf, buf) == 0);

	EXPECT_TRUE(event.Wait().IsSuccess());
}

TEST(CLSocket, UDPServerNonBlockV4)
{
	CLSocket s("3600", SOCKET_TYPE_UDP);

	char buf[10];
	bzero(buf, 10);
	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack(buf, 10).IsSuccess());

	{
		CLSocketAddress addr;
		CLStatus r = s.Read(iov, &addr);
		EXPECT_TRUE(r.m_clReturnCode == -1);
		EXPECT_TRUE(r.m_clErrorCode == IO_PENDING);
	}

	CLEvent event("test_for_socket_tcpserver");

	CLProcess *process = new CLProcess(new CLProcessFunctionForExec);
	EXPECT_TRUE(process->Run((void *)"../test_for_exec/test_for_CLSocketTCPClient/main 4").IsSuccess());

	while(true)
	{
		CLSocketAddress addr;
		CLStatus r = s.Read(iov, &addr);
		if(r.m_clReturnCode == 9)
			break;
	}

	const char *pbuf = "nihaookok";

	EXPECT_TRUE(strcmp(pbuf, buf) == 0);

	EXPECT_TRUE(event.Wait().IsSuccess());
}

TEST(CLSocket, UDPServerBlockV4_Error)
{
	CLSocket s("3600", SOCKET_TYPE_UDP, true);

	CLEvent event("test_for_socket_tcpserver");
	CLEvent event2("test_for_socket_tcpserver_accept_error");

	CLProcess *process = new CLProcess(new CLProcessFunctionForExec);
	EXPECT_TRUE(process->Run((void *)"../test_for_exec/test_for_CLSocketTCPClient/main 5").IsSuccess());

	EXPECT_TRUE(event2.Wait().IsSuccess());

	char buf[10];
	bzero(buf, 10);
	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack(buf, 9).IsSuccess());

	CLSocketAddress addr;
	CLStatus r = s.Read(iov, &addr);
	EXPECT_TRUE(r.m_clReturnCode == 9);
	EXPECT_TRUE(r.m_clErrorCode == 0);
	
	CLStatus r1 = s.Write(iov, &addr);
	EXPECT_TRUE(r1.m_clReturnCode == 9);
	EXPECT_TRUE(r1.m_clErrorCode == 0);

	EXPECT_TRUE(event.Wait().IsSuccess());
}

TEST(CLSocket, UDPServer2BlockV4)
{
	CLSocket s("3600", SOCKET_TYPE_UDP, true);

	CLEvent event("test_for_socket_tcpserver");

	CLProcess *process = new CLProcess(new CLProcessFunctionForExec);
	EXPECT_TRUE(process->Run((void *)"../test_for_exec/test_for_CLSocketTCPClient/main 7").IsSuccess());

	char buf[10];
	bzero(buf, 10);
	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack(buf, 10).IsSuccess());

	CLSocketAddress addr;
	CLStatus r = s.Read(iov, &addr);
	EXPECT_TRUE(r.m_clReturnCode == 9);
	EXPECT_TRUE(r.m_clErrorCode == 0);

	const char *pbuf = "nihaookok";
	EXPECT_TRUE(strcmp(pbuf, buf) == 0);

	CLStatus r1 = s.Write(iov, &addr);
	EXPECT_TRUE(r1.m_clReturnCode == 10);
	EXPECT_TRUE(r1.m_clErrorCode == 0);

	EXPECT_TRUE(event.Wait().IsSuccess());
}

TEST(CLSocket, UDPClientBlockV4)
{
	CLSocket s("127.0.0.1", "3600", SOCKET_TYPE_UDP, true);

	CLEvent event("test_for_socket_tcpserver");
	CLEvent event2("test_for_socket_tcpclient_notify");

	CLProcess *process = new CLProcess(new CLProcessFunctionForExec);
	EXPECT_TRUE(process->Run((void *)"../test_for_exec/test_for_CLSocketTCPClient/main 6").IsSuccess());

	EXPECT_TRUE(event2.Wait().IsSuccess());

	const char *p = "hello";
	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack((char *)p, 5).IsSuccess());
	CLStatus r1 = s.Write(iov);
	EXPECT_TRUE(r1.m_clReturnCode == 5);
	EXPECT_TRUE(r1.m_clErrorCode == 0);

	char buf[10];
	bzero(buf, 10);
	CLIOVectors iov1;
	EXPECT_TRUE(iov1.PushBack(buf, 5).IsSuccess());

	CLStatus r2 = s.Read(iov1);
	EXPECT_TRUE(r2.m_clReturnCode == 5);
	EXPECT_TRUE(r2.m_clErrorCode == 0);

	EXPECT_TRUE(strcmp(p, buf) == 0);

	EXPECT_TRUE(event.Wait().IsSuccess());
}

TEST(CLSocket, GetUuid)
{
	CLSocket s1("4000");
	CLSocket s2("5000");

	CLUuid u1 = s1.GetUuid();
	CLUuid u2 = s2.GetUuid();

	void *p1 = (char *)&u1 + sizeof(long);
	void *p2 = (char *)&u2 + sizeof(long);

	int r = uuid_compare((unsigned char *)p1, (unsigned char *)p2);
	EXPECT_TRUE(r != 0);
}