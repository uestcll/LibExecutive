#include <string.h>
#include "CLSocket.h"
#include "CLTCPListenSocket.h"
#include "CLTCPClientSocket.h"
#include "CLIOVectors.h"
#include "CLLogger.h"
#include "ErrorCode.h"

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

CLSocket::CLSocket(const char *pstrHostNameOrIP, const char *pstrServiceOrPort, bool bBlock)
{
	if((pstrServiceOrPort == 0) || (strlen(pstrServiceOrPort) == 0))
		throw "In CLSocket::CLSocket(), pstrServiceOrPort error";

	if((pstrHostNameOrIP == 0) || (strlen(pstrHostNameOrIP) == 0))
		throw "In CLSocket::CLSocket(), pstrHostNameOrIP error";

	m_pSocket = new CLTCPClientSocket(pstrHostNameOrIP, pstrServiceOrPort, bBlock);
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

CLStatus CLSocket::Connect()
{
	CLTCPClientSocket *p = dynamic_cast<CLTCPClientSocket *>(m_pSocket);
	if(p != 0)
		return p->Connect();
	else
	{
		CLLogger::WriteLogMsg("In CLSocket::Connect(), dynamic_cast error", 0);
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