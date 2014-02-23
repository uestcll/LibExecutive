#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <strings.h>
#include <unistd.h>
#include "CLUDPServerSocket.h"
#include "CLSocket.h"
#include "CLLogger.h"
#include "ErrorCode.h"

CLUDPServerSocket::CLUDPServerSocket(const char *pstrHostNameOrIP, const char *pstrServiceOrPort, bool bBlock) : CLBaseSocket(bBlock)
{
	struct addrinfo hints, *results;

	bzero(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	if(getaddrinfo(pstrHostNameOrIP, pstrServiceOrPort, &hints, &results) != 0)
	{
		CLLogger::WriteLogMsg("In CLUDPServerSocket::CLUDPServerSocket(), getaddrinfo error", errno);
		throw "In CLUDPServerSocket::CLUDPServerSocket(), getaddrinfo error";
	}

	for(struct addrinfo *ptmp = results; ptmp != 0; ptmp = ptmp->ai_next)
	{
		int fd = socket(ptmp->ai_family, ptmp->ai_socktype, ptmp->ai_protocol);
		if(fd == -1)
		{
			CLLogger::WriteLogMsg("In CLUDPServerSocket::CLUDPServerSocket(), socket error", errno);
			continue;
		}

		try
		{
			if(!InitialSocket(fd, bBlock).IsSuccess())
				throw CLStatus(0, 0);

			if(bind(fd, ptmp->ai_addr, ptmp->ai_addrlen) == -1)
			{
				CLLogger::WriteLogMsg("In CLUDPServerSocket::CLUDPServerSocket(), bind error", errno);
				throw CLStatus(0, 0);
			}

			m_SocketFd = fd;

			break;
		}
		catch(CLStatus& s)
		{
			if(close(fd) == -1)
				CLLogger::WriteLogMsg("In CLUDPServerSocket::CLUDPServerSocket(), close error", errno);
		}
	}

	freeaddrinfo(results);

	if(m_SocketFd == -1)
	{
		CLLogger::WriteLogMsg("In CLUDPServerSocket::CLUDPServerSocket(), m_SocketFd == -1", 0);
		throw "In CLUDPServerSocket::CLUDPServerSocket(), m_SocketFd == -1";
	}
}

CLUDPServerSocket::~CLUDPServerSocket()
{
}