#include <gtest/gtest.h>
#include "LibExecutive.h"

TEST(CLDataReceiverByAccept, GetData_Features_Test)
{
	CLLogger::WriteLogMsg("CLDataReceiverByAccept Test", 0);

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

	char pbuf2[10];
	bzero(pbuf2, 10);
	CLIOVectors iov2;
	EXPECT_TRUE(iov2.PushBack(pbuf2, 10).IsSuccess());
	CLStatus r3 = psocket->Read(iov2);
	EXPECT_TRUE(r3.m_clReturnCode == 9);

	EXPECT_TRUE(strcmp(pbuf2, pbuf) == 0);

	delete psocket;

	EXPECT_TRUE(event.Wait().IsSuccess());
}

TEST(CLDataReceiverByAccept, GetData_Pending_Test)
{
	CLSocket *ps = new CLSocket("3600");
	CLDataReceiverByAccept dr(ps);

	CLSocket *psocket = 0;
	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack((char *)(&psocket), sizeof(CLSocket *)).IsSuccess());

	long context = 0;
	CLStatus r1 = dr.GetData(iov, &context);
	EXPECT_TRUE(r1.m_clReturnCode == -1);
	EXPECT_TRUE(r1.m_clErrorCode == RECEIVED_ZERO_BYTE);
	EXPECT_TRUE(psocket == 0);
	EXPECT_TRUE(context == (long)(ps->GetSocket()));
}