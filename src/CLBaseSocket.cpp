#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "CLBaseSocket.h"
#include "CLIOVectors.h"
#include "ErrorCode.h"
#include "CLLogger.h"

CLBaseSocket::CLBaseSocket(bool bBlock)
{
	m_SocketFd = -1;
	m_bBlock = bBlock;
}

CLBaseSocket::CLBaseSocket(int SocketFd, bool bBlock)
{
	if(!InitialSocket(SocketFd, bBlock).IsSuccess())
	{
		if(close(SocketFd) == -1)
			CLLogger::WriteLogMsg("In CLBaseSocket::CLBaseSocket(), close error", errno);

		throw "In CLBaseSocket::CLBaseSocket(), InitialSocket error";
	}

	m_SocketFd = SocketFd;
	m_bBlock = bBlock;
}

CLStatus CLBaseSocket::InitialSocket(int SocketFd, bool bBlock)
{
	int optval = 1;

	if(setsockopt(SocketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1)
	{
		CLLogger::WriteLogMsg("In CLBaseSocket::InitialSocket(), setsockopt error", errno);
		return CLStatus(-1, NORMAL_ERROR);
	}

	if(bBlock = false)
	{
		int val;
		if((val = fcntl(SocketFd, F_GETFL, 0)) < 0)
		{
			CLLogger::WriteLogMsg("In CLBaseSocket::InitialSocket(), fcntl error", errno);
			return CLStatus(-1, NORMAL_ERROR);
		}

		val |= O_NONBLOCK;

		if(fcntl(SocketFd, F_SETFL, val) < 0)
		{
			CLLogger::WriteLogMsg("In CLBaseSocket::InitialSocket(), fcntl F_SETFL error", errno);
			return CLStatus(-1, NORMAL_ERROR);
		}
	}

	return CLStatus(0, 0);
}

CLBaseSocket::~CLBaseSocket()
{
	if(m_SocketFd != -1)
	{
		if(close(m_SocketFd) == -1)
			CLLogger::WriteLogMsg("In CLBaseSocket::~CLBaseSocket(), close error", errno);
	}
}

int CLBaseSocket::GetSocket()
{
	return m_SocketFd;
}

CLStatus CLBaseSocket::Read(CLIOVectors& IOVectors, struct addrinfo *pAddrInfo)
{
	return ReadOrWrite(false, IOVectors, pAddrInfo);
}

CLStatus CLBaseSocket::Write(CLIOVectors& IOVectors, struct addrinfo *pAddrInfo)
{
	return ReadOrWrite(true, IOVectors, pAddrInfo);
}

CLStatus CLBaseSocket::ReadOrWrite(bool bWrite, CLIOVectors& IOVectors, struct addrinfo *pAddrInfo)
{
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