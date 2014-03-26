#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <strings.h>
#include <unistd.h>

#include "CLTCPListenSocket.h"
#include "CLLogger.h"


CLTCPListenSocket::CLTCPListenSocket(const char *pHostNameOrIp, const char *pServiceNameOrPort, bool isBlock, int listenNum) : CLBaseSocket(isBlock)
{
	try
	{
		struct addrinfo hints, results;

		int sockFd;

		bzero(&hints, sizoef(struct addrinfo));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		int ret = getaddrinfo(pHostNameOrIp, pServiceNameOrPort, &hints, &results);
		if(ret != 0)
		{
			CLLogger::WriteLogMsg("In CLTCPListenSocket::CLTCPListenSocket(), getaddrinfo() error", 0);
			throw CLStatus(-1, 0);
		}

		for(struct addrinfo *addr = &results; addr != NULL; addr = addr->ai_next)
		{
			sockFd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
			if(sockFd < 0)
				continue;

			if(!InitSocketFd(sockFd, isBlock).IsSuccess())
			{
				CLLogger::WriteLogMsg("In CLTCPListenSocket::CLTCPListenSocket(), InitSocketFd() error");
				throw  CLStatus(-1, 0);
			}

			if(-1 == bind(sockFd, addr->ai_addr, addr->ai_addrlen))
			{
				CLLogger::WriteLogMsg("In CLTCPListenSocket::CLTCPListenSocket(), bind error", 0)
				throw CLStatus(-1, 0);
			}

			if(-1 == listen(sockFd, listenNum))
			{
				CLLogger::WriteLogMsg("In CLTCPListenSocket::CLTCPListenSocket(), listen error", 0)
				throw CLStatus(-1, 0);
			}
			break;
		}

		m_SocketFd = sockFd;

		if(m_SocketFd < 0)
		{
			CLLogger::WriteLogMsg("In CLTCPListenSocket::CLTCPListenSocket(), sockfd < 0", 0);
			throw CLStatus(-1, 0);
		}
	}
	catch(CLStatus& s)
	{
		close(sockfd);
		throw "In CLTCPListenSocket::CLTCPListenSocket() error";
	}

}

CLTCPListenSocket::~CLTCPListenSocket()
{
	
}

CLStatus CLTCPListenSocket::Accept(CLSocket **ppConnSock, CLSocketAddess **ppOppoAddr)
{
	struct sockaddr addr;
	socklen_t addrlen = sizeof(struct sockaddr);

	int ret = accept(m_SocketFd, &addr, &addrlen);
	if(-1 == ret)
	{
		if((errno == EAGAIN) || (errno == EWOULDBLOCK))
			return CLStatus(-1, ACCEPT_PENDING);

		CLLogger::WriteLogMsg("In CLTCPListenSocket::Accept() error", errno);
		return CLStatus(-1, 0);
	}

	*ppOppoAddr = new CLSocketAddess(addr);

	*ppConnSock = new CLSocket(ret, m_bBlock);

	return CLStatus(0, 0);
}