#include "CLDataReceiverByTCPAccept.h"
#include "CLLogger.h"
#include "CLBuffer.h"
#include "CLStatus.h"
#include "CLIOVector.h"
#include "CLSocket.h"
#include "CLSocketAddress.h"
#include "CLTCPClientArrivedMessage.h"
#include "errCode.h"

CLDataReceiverByTCPAccept::CLDataReceiverByTCPAccept(const char *pHostNameOrIp, const char *pServiceNameOrPort, bool isBlock, int listenNum)
{
	try
	{
		m_pListenSocket = new CLSocket(pHostNameOrIp, pServiceNameOrPort, true, TCP_SOCKET, isBlock, listenNum);
	}
	catch(const char *str)
	{
		throw "In CLDataReceiverByTCPAccept::CLDataReceiverByTCPAccept() ERROR!";
	}
}

CLDataReceiverByTCPAccept::~CLDataReceiverByTCPAccept()
{
	if(m_pListenSocket)
	{
		delete m_pListenSocket;
		m_pListenSocket = NULL;
	}
}
// no break!!!
CLStatus CLDataReceiverByTCPAccept::GetData(CLBuffer *pBuffer)
{
	// CLStatus s2 = m_NamedPipe.ReadVecs(IOVec);
	// if(!s2.IsSuccess())
	// {
	// 	CLLogger::WriteLogMsg("In CLDataReceiverByTCPAccept::GetData(), m_NamedPipe->ReadData error", 0);
	// 	return s2;
	// }
	// long len = s2.m_clReturnCode;
	// readLen += len;

	CLSocket *pTmpConnSock;
	CLSocketAddress *pTmpOppoAddr;

	CLStatus s1 = m_pListenSocket->Accept(&pTmpConnSock, &pTmpOppoAddr);

	if(!s1.IsSuccess())
	{
		if(s1.m_clErrorCode == ACCEPT_PENDING)
			return s1;
	}

	long readLen = 0;

	CLIOVector IOVec;

	CLStatus s2 = pBuffer->GetRestIOVecs(IOVec);
	if(!s2.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLDataReceiverByTCPAccept::GetData(), pBuffer->GetRestIOVecs error", 0);
		return s2;
	}

	CLTCPClientArrivedMessage *pClientArrivedMsg = new CLTCPClientArrivedMessage(pTmpConnSock, pTmpOppoAddr);

	CLStatus s3 = IOVec.WriteData((char *)(&pClientArrivedMsg), 0, sizeof(CLTCPClientArrivedMessage *));
	if(!s3.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLDataReceiverByTCPAccept::GetData(), IOVec.WriteData() error", 0);
		return s3;
	}

	readLen += sizeof(CLTCPClientArrivedMessage *);
	// int connFd = pTmpConnSock->GetSocketFd();

	// CLStatus s3 = IOVec.WriteData((char *)(&connFd), 0, sizeof(int));
	// if(!s3.IsSuccess())
	// {
	// 	CLLogger::WriteLogMsg("In CLDataReceiverByTCPAccept::GetData(), IOVec.WriteData(connfd) error", 0);
	// 	return s3;
	// }

	// readLen = readLen + sizeof(int);

	// CLStatus s4 = IOVec.WriteData((char *)(pTmpOppoAddr->GetAddress()), sizeof(int), pTmpOppoAddr->GetAddressLength());
	// if(!s4.IsSuccess())
	// {
	// 	CLLogger::WriteLogMsg("In CLDataReceiverByTCPAccept::GetData(), IOVec.WriteData(address) error", 0);
	// 	return s4;
	// }	

	// readLen = readLen + pTmpOppoAddr->GetAddressLength();

	// if(pTmpConnSock)
	// 	delete pTmpConnSock;

	// if(pTmpOppoAddr)
	// 	delete pTmpOppoAddr;

	return CLStatus(readLen, 0);
}

const int CLDataReceiverByTCPAccept::GetFd()
{
	return m_pListenSocket->GetSocketFd();
}