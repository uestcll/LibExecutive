#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <strings.h>
#include <unistd.h>

#include "CLTCPClientSocket.h"
#include "CLLogger.h"


CLTCPClientSocket::CLTCPClientSocket(const char *pHostNameOrIp, const char *pServiceNameOrPort, bool isBlock) : CLBaseSocket(isBlock)
{
	m_pHostOrIP = pHostNameOrIp;
	m_pServiceOrPort = pServiceNameOrPort;
	m_pAddressInfo = 0;
}

CLTCPClientSocket::~CLTCPClientSocket()
{

}

CLStatus CLTCPClientSocket::Connect()
{
	struct addrinfo hints, *results;

	int sockFd;

	bzero(&hints, sizoef(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int ret = getaddrinfo(pHostNameOrIp, pServiceNameOrPort, &hints, &results);
	if(ret != 0)
	{
		CLLogger::WriteLogMsg("In CLTCPClientSocket::CLTCPClientSocket(), getaddrinfo() error", 0);
		return CLStatus(-1, 0);
	}

	for(struct addrinfo *addr = results; addr != NULL; addr = addr->ai_next)
	{
		sockFd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if(sockFd < 0)
			continue;

		if(!InitSocketFd(sockFd, isBlock).IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLTCPClientSocket::CLTCPClientSocket(), InitSocketFd() error");
			return  CLStatus(-1, 0);
		}

		if(0 == connect(sockFd, addr->ai_addr, addr->ai_addrlen))
		{
			m_SocketFd = sockFd;

			freeaddrinfo(results);

			return CLStatus(0, 0);
		}
		else if (errno == EINPROGRESS)
		{
			m_SocketFd = sockFd;

			m_pAddressInfo = new struct addrinfo;
			memcpy(m_pAddressInfo, addr, sizeof(struct addrinfo));

			freeaddrinfo(results);

			return CLStatus(-1, CONNECT_PENDING);
		}
		break;
	}

	freeaddrinfo(results);

	if(m_SocketFd < 0)
	{
		CLLogger::WriteLogMsg("In CLTCPClientSocket::CLTCPClientSocket(), sockfd < 0", 0);
		return CLStatus(-1, 0);
	}

	return CLStatus(-1, 0);
}

void  CLTCPClientSocket::NotifyConnectResult(bool bResult)
{
	if(bResult)
	{
		if(m_pAddressInfo)
		{
			delete m_pAddressInfo;
			m_pAddressInfo = 0;
		}
	}
	else
	{
		if(m_SocketFd != -1)
		{
			close(m_SocketFd);
			CLLogger::WriteLogMsg("In CLTCPClientSocket::NotifyConnectResult(), connect error!", 0);
		}
	}

	return;
}