#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include "CLSocket.h"
#include "CLLogger.h"
#include "CLIOVectors.h"
#include "ErrorCode.h"

CLSocket::CLSocket(bool bBlock)
{
	m_SocketFd == -1;
	m_bBlock = bBlock;

	m_pCurrentAddrInfo = 0;
	m_pServerAddrInfoListForClient = 0;
}

CLSocket::CLSocket(const char *pstrServiceOrPort, bool bBlock, const char *pstrHostNameOrIP, int backlog)
{
	if((pstrServiceOrPort == 0) || (strlen(pstrServiceOrPort) == 0))
		throw "In CLSocket::CLSocket(), pstrServiceOrPort error";

	m_SocketFd = -1;
	m_bBlock = bBlock;

	m_pCurrentAddrInfo = 0;
	m_pServerAddrInfoListForClient = 0;

	struct addrinfo hints, *results;

	bzero(&hints, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if(getaddrinfo(pstrHostNameOrIP, pstrServiceOrPort, &hints, &results) != 0)
	{
		CLLogger::WriteLogMsg("In CLSocket::CLSocket(), getaddrinfo error", errno);
		throw "In CLSocket::CLSocket(), getaddrinfo error";
	}

	int optval = 1;

	for(struct addrinfo *ptmp = results; ptmp != 0; ptmp = ptmp->ai_next)
	{
		int fd = socket(ptmp->ai_family, ptmp->ai_socktype, ptmp->ai_protocol);
		if(fd == -1)
		{
			CLLogger::WriteLogMsg("In CLSocket::CLSocket(), socket error", errno);
			continue;
		}

		try
		{
			if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1)
			{
				CLLogger::WriteLogMsg("In CLSocket::CLSocket(), setsockopt error", errno);
				throw CLStatus(0, 0);
			}

			if(bBlock == false)
			{
				if(fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
				{
					CLLogger::WriteLogMsg("In CLSocket::CLSocket(), fcntl error", errno);
					throw CLStatus(0, 0);
				}
			}

			if(bind(fd, ptmp->ai_addr, ptmp->ai_addrlen) == -1)
			{
				CLLogger::WriteLogMsg("In CLSocket::CLSocket(), bind error", errno);
				throw CLStatus(0, 0);
			}

			if(listen(fd, backlog) == -1)
			{
				CLLogger::WriteLogMsg("In CLSocket::CLSocket(), listen error", errno);
				throw CLStatus(0, 0);
			}

			m_SocketFd = fd;

			break;
		}
		catch(CLStatus& s)
		{
			if(close(fd) == -1)
				CLLogger::WriteLogMsg("In CLSocket::CLSocket(), close error", errno);
		}
	}

	freeaddrinfo(results);

	if(m_SocketFd == -1)
	{
		CLLogger::WriteLogMsg("In CLSocket::CLSocket(), m_SocketFd == -1", 0);
		throw "In CLSocket::CLSocket(), m_SocketFd == -1";
	}
}

CLSocket::CLSocket(int SocketFd, bool bBlock)
{
	if(SocketFd < 0)
		throw "In CLSocket::CLSocket2(), SocketFd error";

	m_SocketFd = SocketFd;
	m_bBlock = bBlock;

	m_pCurrentAddrInfo = 0;
	m_pServerAddrInfoListForClient = 0;

	try
	{
		int optval = 1;

		if(setsockopt(SocketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1)
		{
			CLLogger::WriteLogMsg("In CLSocket::CLSocket2(), setsockopt error", errno);
			throw "In CLSocket::CLSocket2(), setsockopt error";
		}

		if(bBlock = false)
		{
			int val;
			if((val = fcntl(SocketFd, F_GETFL, 0)) < 0)
			{
				CLLogger::WriteLogMsg("In CLSocket::CLSocket2(), fcntl error", errno);
				throw "In CLSocket::CLSocket2(), fcntl error";
			}

			val |= O_NONBLOCK;

			if(fcntl(SocketFd, F_SETFL, val) < 0)
			{
				CLLogger::WriteLogMsg("In CLSocket::CLSocket2(), fcntl 2 error", errno);
				throw "In CLSocket::CLSocket2(), fcntl 2 error";
			}
		}
	}
	catch(const char *str)
	{
		if(close(SocketFd) == -1)
			CLLogger::WriteLogMsg("In CLSocket::CLSocket2(), close error", errno);

		throw str;
	}
}

CLSocket::~CLSocket()
{
	if(m_pServerAddrInfoListForClient != 0)
		freeaddrinfo(m_pServerAddrInfoListForClient);

	if(m_SocketFd != -1)
	{
		if(close(m_SocketFd) == -1)
			CLLogger::WriteLogMsg("In CLSocket::~CLSocket(), close error", errno);
	}
}

int CLSocket::GetSocket()
{
	return m_SocketFd;
}

CLStatus CLSocket::Accept(CLSocket **ppSocket)
{
	if(ppSocket == 0)
		return CLStatus(-1, NORMAL_ERROR);

	int r = accept(m_SocketFd, 0, 0);

	if(r == -1)
		return CLStatus(-1, errno);

	*ppSocket = new CLSocket(r, m_bBlock);

	return CLStatus(0, 0);
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
	return ReadOrWrite(false, IOVectors, pAddrInfo);
}

CLStatus CLSocket::Write(CLIOVectors& IOVectors, struct addrinfo *pAddrInfo)
{
	return ReadOrWrite(true, IOVectors, pAddrInfo);
}

CLStatus CLSocket::ReadOrWrite(bool bWrite, CLIOVectors& IOVectors, struct addrinfo *pAddrInfo)
{
	if(IOVectors.Size() == 0)
		return CLStatus(-1, NORMAL_ERROR);

	struct msghdr msg;

	if(pAddrInfo == 0)
	{
		msg.msg_name = 0;
		msg.msg_namelen = 0;
	}
	else
	{
		msg.msg_name = pAddrInfo->ai_addr;
		msg.msg_namelen = pAddrInfo->ai_addrlen;
	}

	struct iovec *iov = IOVectors.GetIOVecArray();
	if(iov == 0)
		return CLStatus(-1, NORMAL_ERROR);

	msg.msg_iov = iov;
	msg.msg_iovlen = IOVectors.GetNumberOfIOVec();

	msg.msg_control = 0;
	msg.msg_controllen = 0;
	msg.msg_flags = 0;

	ssize_t result;
	if(bWrite)
		result = sendmsg(m_SocketFd, &msg, 0);
	else
		result = recvmsg(m_SocketFd, &msg, 0);

	int err = errno;

	delete [] iov;

	return CLStatus(result, err);
}