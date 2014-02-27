#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include "CLBaseSocket.h"
#include "CLIOVectors.h"
#include "CLSocketAddress.h"
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

	if(bBlock == false)
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

CLStatus CLBaseSocket::Read(CLIOVectors& IOVectors, CLSocketAddress *pSocketAddress)
{
	return ReadOrWrite(false, IOVectors, pSocketAddress);
}

CLStatus CLBaseSocket::Write(CLIOVectors& IOVectors, CLSocketAddress *pSocketAddress)
{
	return ReadOrWrite(true, IOVectors, pSocketAddress);
}

CLStatus CLBaseSocket::ReadOrWrite(bool bWrite, CLIOVectors& IOVectors, CLSocketAddress *pSocketAddress)
{
	struct msghdr msg;

	if(pSocketAddress == 0)
	{
		msg.msg_name = 0;
		msg.msg_namelen = 0;
	}
	else
	{
		msg.msg_name = pSocketAddress->GetAddress();
		msg.msg_namelen = pSocketAddress->GetAddressLength();
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

	int err;

	if(result == -1)
	{
		if((errno == EAGAIN) || (errno == EWOULDBLOCK))
			err = IO_PENDING;
		else
		{
			CLLogger::WriteLogMsg("In CLBaseSocket::ReadOrWrite(), sendmsg or recvmsg error", errno);
			err = NORMAL_ERROR;
		}
	}
	else
		err = 0;

	delete [] iov;

	return CLStatus(result, err);
}