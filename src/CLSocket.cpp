#include "CLSocket.h"
#include "CLIOVector.h"
#include "CLBaseSocket.h"
#include "CLTCPListenSocket.h"
#include "CLUDPServerSocket.h"
#include "CLTCPClientSocket.h"
#include "CLUDPClientSocket.h"
#include "CLLogger.h"
#include "CLSocketAddress.h"

CLSocket::CLSocket(const char *pHostNameOrIp, const char *pServiceNameOrPort, bool forServer, int sockeType, bool isBlock, int listenNum)
{
	if(!pHostNameOrIp || !pServiceNameOrPort)
		throw "In CLSocket::CLSocket(): service or host name error";

	if(sockeType == TCP_SOCKET)
	{
		if(forServer)
			m_pSocket = new CLTCPListenSocket(pHostNameOrIp, pServiceNameOrPort, isBlock, listenNum);
		else
			m_pSocket = new CLTCPClientSocket(pHostNameOrIp, pServiceNameOrPort, isBlock);
	}
	else if(sockeType == UDP_SOCKET)
	{
		if(forServer)
			m_pSocket = new CLUDPServerSocket(pHostNameOrIp, pServiceNameOrPort, isBlock);
		else
			m_pSocket = new CLUDPClientSocket(pHostNameOrIp, pServiceNameOrPort, isBlock);
	}
	else
	{
		throw "In CLSocket::CLSocket(): socket type error!";
	}
}

CLSocket::CLSocket(int sockFd, bool isBlock = true)
{
	if(sockFd <= 0)
		throw "In CLSocket::CLSocket(): sockFd error";

	m_pSocket = new CLBaseSocket(sockFd, isBlock);
}


CLSocket::~CLSocket()
{
	if(m_pSocket)
		delete m_pSocket;
}

CLStatus CLSocket::Accept(CLSocket **ppConnSock, CLSocketAddress **pOppoAddress)
{
	if(!ppConnSock || !m_pSocket)
	{
		return CLStatus(-1, 0);
	}

	CLTCPListenSocket *pTmpSocket = (CLTCPListenSocket *)dynamic_cast<CLTCPListenSocket *> (m_pSocket);
	if(pTmpSocket)
		return pTmpSocket->Accept(ppConnSock, pOppoAddress);
	else
	{
		CLLogger::WriteLogMsg("In CLSocket::Accept(), dynamic_cast error", 0);
		return CLStatus(-1, 0);
	}
}

CLStatus CLSocket::Connect()
{
	if(!m_pSocket)
	{
		return CLStatus(-1, 0);
	}

	CLTCPClientSocket* pTmpSocket = (CLTCPClientSocket *)dynamic_cast<CLTCPClientSocket *> (m_pSocket);
	if(pTmpSocket)
		return pTmpSocket->Connect();
	else
	{
		CLLogger::WriteLogMsg("In CLSocket::Connect(), dynamic_cast error", 0);
		return CLStatus(-1, 0);
	}
}

CLStatus CLSocket::Read(CLIOVector& IOVec, CLSocketAddress* pAddr)
{
	if(IOVec.IOVecNum() == 0)
		return CLStatus(-1, 0);

	return m_pSocket->Read(IOVec, pAddr);
}

CLStatus CLSocket::Write(CLIOVector& IOVec, CLSocketAddress* pAddr)
{
	if(IOVec.IOVecNum() == 0)
		return CLStatus(-1, 0);

	return m_pSocket->Write(IOVec, pAddr);
}

const int CLSocket::GetSocketFd() const
{
	return m_pSocket->GetSocketFd();
}

void  CLSocket::NotifyConnectResult(bool bResult)
{
	if(!m_pSocket)
	{
		return;
	}

	CLTCPClientSocket* pTmpSocket = (CLTCPClientSocket *)dynamic_cast<CLTCPClientSocket *> (m_pSocket);
	if(pTmpSocket)
	{	
		pTmpSocket->NotifyConnectResult(bResult);
		return;
	}
	else
	{
		CLLogger::WriteLogMsg("In CLSocket::NotifyConnectResult(), dynamic_cast error", 0);
		return;
	}
}
