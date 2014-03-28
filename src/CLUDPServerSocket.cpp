#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <strings.h>
#include <unistd.h>
#include "CLLogger.h"
#include "CLUDPServerSocket.h"

CLUDPServerSocket::CLUDPServerSocket(const char *pHostNameOrIp, const char *pServiceNameOrPort, bool isBlock) : CLBaseSocket(isBlock)
{
	struct addrinfo hints, *results;

	int sockFd;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	int ret = getaddrinfo(pHostNameOrIp, pServiceNameOrPort, &hints, &results);
	if(ret != 0)
	{
		CLLogger::WriteLogMsg("In CLTCPClientSocket::CLTCPClientSocket(), getaddrinfo() error", 0);
		throw CLStatus(-1, 0);
	}

	for(struct addrinfo *addr = results; addr != NULL; addr = addr->ai_next)
	{
		sockFd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if(sockFd < 0)
			continue;

		try
		{
			if(!InitSocketFd(sockFd, isBlock).IsSuccess())
			{
				CLLogger::WriteLogMsg("In CLTCPClientSocket::CLTCPClientSocket(), InitSocketFd() error", 0);
				throw  CLStatus(-1, 0);
			}

			if(-1 == bind(sockFd, addr->ai_addr, addr->ai_addrlen))
			{
				CLLogger::WriteLogMsg("In CLTCPClientSocket::CLTCPClientSocket(), bind error", 0);
				throw CLStatus(-1, 0);
			}

			m_SocketFd = sockFd;

			break;
		}
		catch(CLStatus &)
		{
			close(sockFd);
		}
				
	}

	freeaddrinfo(results);

	if(m_SocketFd < 0)
	{
		CLLogger::WriteLogMsg("In CLTCPClientSocket::CLTCPClientSocket(), sockfd < 0", 0);
		throw "In CLUDPServerSocket::CLUDPServerSocket(),m_SocketFd < 0";
	}
}

CLUDPServerSocket::~CLUDPServerSocket()
{
	
}