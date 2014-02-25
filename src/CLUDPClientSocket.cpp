#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <strings.h>
#include "CLUDPClientSocket.h"
#include "ErrorCode.h"
#include "CLLogger.h"

CLUDPClientSocket::CLUDPClientSocket(const char *pstrHostNameOrIP, const char *pstrServiceOrPort, bool bBlock) : CLBaseSocket(bBlock)
{
	struct addrinfo hints, *results;

	bzero(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if(getaddrinfo(pstrHostNameOrIP, pstrServiceOrPort, &hints, &results) != 0)
	{
		CLLogger::WriteLogMsg("In CLUDPClientSocket::CLUDPClientSocket(), getaddrinfo error", errno);
		throw "In CLUDPClientSocket::CLUDPClientSocket(), getaddrinfo error";
	}

	for(struct addrinfo *ptmp = results; ptmp != 0; ptmp = ptmp->ai_next)
	{
		int fd = socket(ptmp->ai_family, ptmp->ai_socktype, ptmp->ai_protocol);
		if(fd == -1)
		{
			CLLogger::WriteLogMsg("In CLUDPClientSocket::CLUDPClientSocket(), socket error", errno);
			continue;
		}

		try
		{
			if(!InitialSocket(fd, bBlock).IsSuccess())
				throw CLStatus(0, 0);

			if(connect(fd, ptmp->ai_addr, ptmp->ai_addrlen) == -1)
			{
				CLLogger::WriteLogMsg("In CLUDPClientSocket::CLUDPClientSocket(), connect error", errno);
				throw CLStatus(0, 0);
			}

			m_SocketFd = fd;

			break;
		}
		catch(CLStatus& s)
		{
			if(close(fd) == -1)
				CLLogger::WriteLogMsg("In CLUDPClientSocket::CLUDPClientSocket(), close error", errno);
		}
	}

	freeaddrinfo(results);

	if(m_SocketFd == -1)
	{
		CLLogger::WriteLogMsg("In CLUDPClientSocket::CLUDPClientSocket(), m_SocketFd == -1", 0);
		throw "In CLUDPClientSocket::CLUDPClientSocket(), m_SocketFd == -1";
	}
}

CLUDPClientSocket::~CLUDPClientSocket()
{
}