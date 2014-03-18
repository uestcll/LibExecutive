#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "CLBaseSocket.h"
#include "CLLogger.h"

CLBaseSocket::CLBaseSocket(bool isBlock) : m_SocketFd(-1), m_bBlock(isBlock)
{

}

CLBaseSocket::CLBaseSocket(int fd, bool isBlock)
{
	m_SocketFd = fd;
	m_bBlock = isBlock;

	if(!InitSocketFd(m_SocketFd, isBlock).IsSuccess())
	{
		close(m_SocketFd);
		CLLogger::WriteLogMsg("In CLBaseSocket::CLBaseSocket(), InitSocketFd() error", 0);
		throw "In CLBaseSocket::CLBaseSocket(int fd, bool isBlock), InitSocketFd() error";
	}
}

CLBaseSocket::~CLBaseSocket()
{
	if(m_SocketFd > 0)
		close(m_SocketFd);
}

CLStatus CLBaseSocket::InitSocketFd(int fd, bool isBlock)
{
	if(!isBlock)
	{
		int flag = fcntl(fd, F_GETFL, 0);
		if(flag < 0)
		{
			CLLogger::WriteLogMsg("In CLBaseSocket::InitSocketFd(), fcntl get flag error", 0);
			return CLStatus(-1, 0);
		}

		flag |= O_NONBLOCK;

		if(fcntl(fd, F_SETFL, flag) < 0)
		{
			CLLogger::WriteLogMsg("In CLBaseSocket::InitSocketFd(), fcntl set flag error", 0);
			return CLStatus(-1, 0);
		}
	}


	return CLStatus(0, 0);
}

CLStatus CLBaseSocket::Read(CLIOVector& IOVec, CLSocketAddress *pAddr)
{
	return ReadOrWrite(IOVec, pAddr, false);
}

CLStatus CLBaseSocket::Write(CLIOVector& IOVec, CLSocketAddress *pAddr)
{
	return ReadOrWrite(IOV>, pAddr, true);
}

CLStatus CLBaseSocket::ReadOrWrite(CLIOVector& IOVec, CLSocketAddress *pAddr, bool bWrite)
{
	struct msghdr msg;
	
	memset(&msg, 0, sizeof(struct msghdr));

	if(pAddr)
	{
		msg.msg_name = pAddr->GetAddress();
		msg.msg_namelen = pAddr->GetAddressLength();
	}

	struct iovec *iov = IOVec->GetIOVecStructs();
	if(iov == NULL)
	{
		CLLogger::WriteLogMsg("In CLBaseSocket::ReadOrWrite(), iov is NULL", 0);
		return CLSocketAddress(-1, 0);
	}

	msg.msg_iov = iov; 
	msg.msg_iovlen = IOVec->IOVecNum();

	ssize_t length;

	if(bWrite)
		length	= sendmsg(m_SocketFd, &msg, 0);
	else
		length = recvmsg(m_SocketFd, &msg, 0);

	int ret = 0;

	if(0 == length)
	{
		ret = IO_PAUSE;
	}
	else if(-1 == length)
	{
		if((errno == EAGAIN) || (errno == EWOULDBLOCK))
			ret = IO_PENDING;
		else
		{
			CLLogger::WriteLogMsg("In CLBaseSocket::ReadOrWrite(), length == -1", 0);
			ret = -1;
		}
	}

	delete [] iov;

	return CLStatus(length, ret);
}

const int CLBaseSocket::GetSocketFd() const
{
	return m_SocketFd;
}
