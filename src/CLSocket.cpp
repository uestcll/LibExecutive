#include <string.h>
#include "CLSocket.h"
#include "CLTCPListenSocket.h"
#include "CLTCPClientSocket.h"
#include "CLUDPServerSocket.h"
#include "CLUDPClientSocket.h"
#include "CLIOVectors.h"
#include "CLLogger.h"
#include "ErrorCode.h"

CLSocket::CLSocket(const char *pstrServiceOrPort, int SocketType, bool bBlock, const char *pstrHostNameOrIP, int backlog)
{
	if((pstrServiceOrPort == 0) || (strlen(pstrServiceOrPort) == 0))
		throw "In CLSocket::CLSocket(), pstrServiceOrPort error";

	uuid_generate(m_UuidOfSocket);

	if(SOCKET_TYPE_TCP == SocketType)
		m_pSocket = new CLTCPListenSocket(pstrHostNameOrIP, pstrServiceOrPort, backlog, bBlock);
	else if(SOCKET_TYPE_UDP == SocketType)
		m_pSocket = new CLUDPServerSocket(pstrHostNameOrIP, pstrServiceOrPort, bBlock);
	else
		throw "In CLSocket::CLSocket(), SocketType error";
}

CLSocket::CLSocket(int SocketFd, bool bBlock)
{
	if(SocketFd < 0)
		throw "In CLSocket::CLSocket(), SocketFd error";

	uuid_generate(m_UuidOfSocket);

	m_pSocket = new CLBaseSocket(SocketFd, bBlock);
}

CLSocket::CLSocket(const char *pstrHostNameOrIP, const char *pstrServiceOrPort, int SocketType, bool bBlock)
{
	if((pstrServiceOrPort == 0) || (strlen(pstrServiceOrPort) == 0))
		throw "In CLSocket::CLSocket(), pstrServiceOrPort error";

	if((pstrHostNameOrIP == 0) || (strlen(pstrHostNameOrIP) == 0))
		throw "In CLSocket::CLSocket(), pstrHostNameOrIP error";

	uuid_generate(m_UuidOfSocket);

	if(SOCKET_TYPE_TCP == SocketType)
		m_pSocket = new CLTCPClientSocket(pstrHostNameOrIP, pstrServiceOrPort, bBlock);
	else if(SOCKET_TYPE_UDP == SocketType)
		m_pSocket = new CLUDPClientSocket(pstrHostNameOrIP, pstrServiceOrPort, bBlock);
	else
		throw "In CLSocket::CLSocket() 2, SocketType error";
}

CLSocket::~CLSocket()
{
	delete m_pSocket;
}

int CLSocket::GetSocket()
{
	return m_pSocket->GetSocket();
}

void CLSocket::GetUuid(uuid_t uuid)
{
	uuid_copy(uuid, m_UuidOfSocket);
	//...........
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

void CLSocket::NotifyConnectResults(bool bResults)
{
	CLTCPClientSocket *p = dynamic_cast<CLTCPClientSocket *>(m_pSocket);
	if(p != 0)
		return p->NotifyConnectResults(bResults);
	else
	{
		CLLogger::WriteLogMsg("In CLSocket::NotifyConnectResults(), dynamic_cast error", 0);
		return;
	}
}

CLStatus CLSocket::Read(CLIOVectors& IOVectors, CLSocketAddress *pSocketAddress)
{
	if(IOVectors.Size() == 0)
		return CLStatus(-1, NORMAL_ERROR);

	return m_pSocket->Read(IOVectors, pSocketAddress);
}

CLStatus CLSocket::Write(CLIOVectors& IOVectors, CLSocketAddress *pSocketAddress)
{
	if(IOVectors.Size() == 0)
		return CLStatus(-1, NORMAL_ERROR);

	return m_pSocket->Write(IOVectors, pSocketAddress);
}