#include <iostream>
#include <arpa/inet.h>
#include <gtest/gtest.h>
#include "LibExecutive.h"

TEST(CLSocket, CLSocket)
{
	CLSocket *pSocket = new CLSocket("localhost", "2323");
	EXPECT_TRUE(pSocket->GetSocketFd());
	CLSocket *pClientSocket = new CLSocket("localhost", "2323", false);
	//EXPECT_TRUE((pClientSocket->Connect()).IsSuccess());
	EXPECT_TRUE(pSocket->GetSocketFd());
	CLSocket *pConnSocket;
	CLSocketAddress *pConnAddr;

//	CLStatus s = pSocket->Accept(&pConnSocket, &pConnAddr);
//	EXPECT_TRUE(s.IsSuccess());
//	EXPECT_TRUE(pConnSocket->GetSocket());

	//std::cout<<inet_ntoa(((struct sockaddr_in*)pConnAddr->GetAddress())->sin_addr)<<" "<<s.m_clErrorCode<<std::endl;
}
