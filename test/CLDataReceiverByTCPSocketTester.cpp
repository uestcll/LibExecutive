#include <gtest/gtest.h>
#include <unistd.h>
#include "LibExecutive.h"

TEST(CLDataReceiverByTCPSocket, GetData_Features_Test)
{
	CLLogger::WriteLogMsg("CLDataReceiverByTCPSocket Test", 0);

	CLSocket *ps = new CLSocket("3600", SOCKET_TYPE_TCP, true);
	CLDataReceiverByAccept dr(ps);

	CLEvent event("test_for_socket_tcpserver");
	CLProcess *process = new CLProcess(new CLProcessFunctionForExec);
	EXPECT_TRUE(process->Run((void *)"../test_for_exec/test_for_CLSocketTCPClient/main 1").IsSuccess());

	CLSocket *psocket = 0;
	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack((char *)(&psocket), sizeof(CLSocket *)).IsSuccess());
	
	long context = 0;
	CLStatus r1 = dr.GetData(iov, &context);
	EXPECT_TRUE(r1.m_clReturnCode == sizeof(CLSocket *));
	EXPECT_TRUE(psocket != 0);
	EXPECT_TRUE(context == (long)(ps->GetSocket()));

	const char *pbuf = "nihaookok";
	CLIOVectors iov1;
	EXPECT_TRUE(iov1.PushBack((char *)pbuf, 9).IsSuccess());
	CLStatus r2 = psocket->Write(iov1);
	EXPECT_TRUE(r2.m_clReturnCode == 9);

	CLDataReceiverByTCPSocket dt(psocket);

	context = 0;
	char pbuf2[10];
	bzero(pbuf2, 10);
	CLIOVectors iov2;

	CLStatus r4 = dt.GetData(iov2, &context);
	EXPECT_TRUE(r4.m_clReturnCode == -1);
	EXPECT_TRUE(r4.m_clErrorCode == NORMAL_ERROR);

	EXPECT_TRUE(iov2.PushBack(pbuf2, 10).IsSuccess());
	CLStatus r3 = dt.GetData(iov2, &context);
	EXPECT_TRUE(r3.m_clReturnCode == 9);
	EXPECT_TRUE(context == (long)psocket->GetSocket());

	EXPECT_TRUE(strcmp(pbuf2, pbuf) == 0);

	CLStatus r5 = dt.GetData(iov2, &context);
	EXPECT_TRUE(r5.m_clReturnCode == -1);
	EXPECT_TRUE(r5.m_clErrorCode == RECEIVED_CLOSE);

	EXPECT_TRUE(event.Wait().IsSuccess());
}

TEST(CLDataReceiverByTCPSocket, GetData_Pending_Test)
{
	CLSocket *ps = new CLSocket("3600");
	CLDataReceiverByAccept dr(ps);

	CLEvent event("test_for_socket_tcpserver");
	CLEvent event2("test_for_CLDataReceiverByTCPSocket_Pending");
	CLProcess *process = new CLProcess(new CLProcessFunctionForExec);
	EXPECT_TRUE(process->Run((void *)"../test_for_exec/test_for_CLSocketTCPClient/main 8").IsSuccess());

	EXPECT_TRUE(event2.Wait().IsSuccess());

	CLSocket *psocket = 0;
	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack((char *)(&psocket), sizeof(CLSocket *)).IsSuccess());
	long context = 0;
	CLStatus r1 = dr.GetData(iov, &context);
	EXPECT_TRUE(r1.m_clReturnCode == sizeof(CLSocket *));
	EXPECT_TRUE(psocket != 0);
	EXPECT_TRUE(context == (long)(ps->GetSocket()));

	const char *pbuf = "nihaookok";
	CLIOVectors iov1;
	EXPECT_TRUE(iov1.PushBack((char *)pbuf, 9).IsSuccess());
	CLStatus r2 = psocket->Write(iov1);
	EXPECT_TRUE(r2.m_clReturnCode == 9);

	CLDataReceiverByTCPSocket dt(psocket);

	context = 0;
	char pbuf2[10];
	bzero(pbuf2, 10);
	CLIOVectors iov2;
	EXPECT_TRUE(iov2.PushBack(pbuf2, 10).IsSuccess());
	CLStatus r3 = dt.GetData(iov2, &context);
	EXPECT_TRUE(r3.m_clReturnCode == -1);
	EXPECT_TRUE(r3.m_clErrorCode == RECEIVED_ZERO_BYTE);
	EXPECT_TRUE(context == (long)psocket->GetSocket());

	EXPECT_TRUE(event.Wait().IsSuccess());
}

TEST(CLDataReceiverByTCPSocket, GetData_Error_Test)
{
	CLSocket *ps = new CLSocket("192.157.2.4", "3600");

	CLStatus r = ps->Connect();
	EXPECT_TRUE(r.m_clReturnCode == -1);
	EXPECT_TRUE(r.m_clErrorCode == CONNECT_PENDING);

	long context = 0;
	char pbuf2[10];
	bzero(pbuf2, 10);
	CLIOVectors iov2;
	EXPECT_TRUE(iov2.PushBack(pbuf2, 10).IsSuccess());

	CLLogger::WriteLogMsg("2 The Following bug is produced on purpose", 0);

	sleep(60);

	CLDataReceiverByTCPSocket dt(ps);
	CLStatus r3 = dt.GetData(iov2, &context);
	EXPECT_TRUE(r3.m_clReturnCode == -1);
	EXPECT_TRUE(r3.m_clErrorCode == NORMAL_ERROR);
	EXPECT_TRUE(context == (long)ps->GetSocket());
}