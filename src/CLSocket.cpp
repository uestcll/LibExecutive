#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include "CLSocket.h"
#include "CLLogger.h"

CLSocket::CLSocket(bool bBlock, int family, int type)
{
	m_SocketFd = socket(family, type, 0);
	if(m_SocketFd == -1)
		throw "In CLSocket::CLSocket(), socket error";

	int optval = 1;
	if(setsockopt(m_SocketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1)
		CLLogger::WriteLogMsg("In CLSocket::CLSocket(), setsockopt error", errno);

	if(bBlock == false)
	{
		if(fcntl(m_SocketFd, F_SETFL, O_NONBLOCK) == -1)
			CLLogger::WriteLogMsg("In CLSocket::CLSocket(), fcntl error", errno);
	}
}

CLSocket::~CLSocket()
{
	if(close(m_SocketFd) == -1)
		CLLogger::WriteLogMsg("In CLSocket::~CLSocket(), close error", errno);
}

int CLSocket::GetSocket()
{
	return m_SocketFd;
}

CLStatus CLSocket::InitialServer(int port, int backlog, const char *strBoundIP)
{
	sockaddr_storage ServerAddress;
}