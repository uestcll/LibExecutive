#include <string.h>
#include "CLSocket.h"
#include "CLTCPListenSocket.h"
#include "CLIOVectors.h"
#include "CLLogger.h"
#include "ErrorCode.h"

CLSocket::CLSocket(bool bBlock)
{
}

CLSocket::CLSocket(const char *pstrServiceOrPort, bool bBlock, const char *pstrHostNameOrIP, int backlog)
{
	if((pstrServiceOrPort == 0) || (strlen(pstrServiceOrPort) == 0))
		throw "In CLSocket::CLSocket(), pstrServiceOrPort error";

	m_pSocket = new CLTCPListenSocket(pstrHostNameOrIP, pstrServiceOrPort, backlog, bBlock);
}

CLSocket::CLSocket(int SocketFd, bool bBlock)
{
	if(SocketFd < 0)
		throw "In CLSocket::CLSocket(), SocketFd error";

	m_pSocket = new CLBaseSocket(SocketFd, bBlock);
}

CLSocket::~CLSocket()
{
	delete m_pSocket;
}

int CLSocket::GetSocket()
{
	return m_pSocket->GetSocket();
}

CLStatus CLSocket::Accept(CLSocket **ppSocket)
{
	if(ppSocket == 0)
		return CLStatus(-1, NORMAL_ERROR);

	CLTCPListenSocket *p = dynamic_cast<CLTCPListenSocket *>(m_pSocket);
	if(p != 0)
		return p->Accept(ppSocket);
	else
	{
		CLLogger::WriteLogMsg("In CLSocket::Accept(), dynamic_cast error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}
}

CLStatus CLSocket::Connect(const char *pstrHostNameOrIP, const char *pstrServiceOrPort)
{
	if(m_SocketFd != -1)
		return CLStatus(-1, NORMAL_ERROR);

	if(m_pServerAddrInfoListForClient == 0)
	{
		struct addrinfo hints;

		bzero(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		if(getaddrinfo(pstrHostNameOrIP, pstrServiceOrPort, &hints, &m_pServerAddrInfoListForClient) != 0)
		{
			CLLogger::WriteLogMsg("In CLSocket::Connect(), getaddrinfo error", errno);
			return CLStatus(-1, NORMAL_ERROR);
		}

		m_pCurrentAddrInfo = m_pServerAddrInfoListForClient;
	}

	if(m_pCurrentAddrInfo == 0)
		return CLStatus(-1, ***);

	
	m_pCurrentAddrInfo = m_pCurrentAddrInfo->ai_next;

	for(struct addrinfo *ptmp = results; ptmp != 0; ptmp = ptmp->ai_next)
	{
		int fd = socket(ptmp->ai_family, ptmp->ai_socktype, ptmp->ai_protocol);
		if(fd == -1)
		{
			CLLogger::WriteLogMsg("In CLSocket::Connect(), socket error", errno);
			continue;
		}

		//......................
	}

	freeaddrinfo(results);

	if(m_SocketFd == -1)
	{
		CLLogger::WriteLogMsg("In CLSocket::Connect(), m_SocketFd == -1", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}
}

CLStatus CLSocket::Read(CLIOVectors& IOVectors, struct addrinfo *pAddrInfo)
{
	if(IOVectors.Size() == 0)
		return CLStatus(-1, NORMAL_ERROR);

	return m_pSocket->Read(IOVectors, pAddrInfo);
}

CLStatus CLSocket::Write(CLIOVectors& IOVectors, struct addrinfo *pAddrInfo)
{
	if(IOVectors.Size() == 0)
		return CLStatus(-1, NORMAL_ERROR);

	return m_pSocket->Write(IOVectors, pAddrInfo);
}