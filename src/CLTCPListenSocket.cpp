#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <strings.h>
#include <unistd.h>
#include "CLTCPListenSocket.h"
#include "CLSocket.h"
#include "CLLogger.h"
#include "ErrorCode.h"

CLTCPListenSocket::CLTCPListenSocket(const char *pstrHostNameOrIP, const char *pstrServiceOrPort, int backlog, bool bBlock) : CLBaseSocket(bBlock)
{
	struct addrinfo hints, *results;

	bzero(&hints, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if(getaddrinfo(pstrHostNameOrIP, pstrServiceOrPort, &hints, &results) != 0)
	{
		CLLogger::WriteLogMsg("In CLTCPListenSocket::CLTCPListenSocket(), getaddrinfo error", errno);
		throw "In CLTCPListenSocket::CLTCPListenSocket(), getaddrinfo error";
	}

	for(struct addrinfo *ptmp = results; ptmp != 0; ptmp = ptmp->ai_next)
	{
		int fd = socket(ptmp->ai_family, ptmp->ai_socktype, ptmp->ai_protocol);
		if(fd == -1)
		{
			CLLogger::WriteLogMsg("In CLTCPListenSocket::CLTCPListenSocket(), socket error", errno);
			continue;
		}

		try
		{
			if(!InitialSocket(fd, bBlock).IsSuccess())
				throw CLStatus(0, 0);

			if(bind(fd, ptmp->ai_addr, ptmp->ai_addrlen) == -1)
			{
				CLLogger::WriteLogMsg("In CLTCPListenSocket::CLTCPListenSocket(), bind error", errno);
				throw CLStatus(0, 0);
			}

			if(listen(fd, backlog) == -1)
			{
				CLLogger::WriteLogMsg("In CLTCPListenSocket::CLTCPListenSocket(), listen error", errno);
				throw CLStatus(0, 0);
			}

			m_SocketFd = fd;

			break;
		}
		catch(CLStatus& s)
		{
			if(close(fd) == -1)
				CLLogger::WriteLogMsg("In CLTCPListenSocket::CLTCPListenSocket(), close error", errno);
		}
	}

	freeaddrinfo(results);

	if(m_SocketFd == -1)
	{
		CLLogger::WriteLogMsg("In CLTCPListenSocket::CLTCPListenSocket(), m_SocketFd == -1", 0);
		throw "In CLTCPListenSocket::CLTCPListenSocket(), m_SocketFd == -1";
	}
}

CLTCPListenSocket::~CLTCPListenSocket()
{
}

CLStatus CLTCPListenSocket::Accept(CLSocket **ppSocket)
{
	int r = accept(m_SocketFd, 0, 0);
	if(r == -1)
	{
		if((errno == EAGAIN) || (errno == EWOULDBLOCK))
			return CLStatus(-1, ACCEPT_PENDING);

		CLLogger::WriteLogMsg("In CLTCPListenSocket::Accept(), accept error", errno);

		return CLStatus(-1, NORMAL_ERROR);
	}

	*ppSocket = new CLSocket(r, m_bBlock);

	return CLStatus(0, 0);
}
