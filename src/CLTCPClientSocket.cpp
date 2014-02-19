#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <strings.h>
#include "CLTCPClientSocket.h"
#include "ErrorCode.h"
#include "CLLogger.h"

CLTCPClientSocket::CLTCPClientSocket(const char *pstrHostNameOrIP, const char *pstrServiceOrPort, bool bBlock) : CLBaseSocket(bBlock)
{
	m_strServiceOrPort = pstrServiceOrPort;
	m_strHostNameOrIP = pstrHostNameOrIP;

	m_pServerAddrInfoListForClient = 0;
	m_pCurrentAddrInfo = 0;
}

CLTCPClientSocket::~CLTCPClientSocket()
{
	if(m_pServerAddrInfoListForClient != 0)
		freeaddrinfo(m_pServerAddrInfoListForClient);
}

CLStatus CLTCPClientSocket::Connect()
{
	if(m_SocketFd != -1)
		return CLStatus(-1, NORMAL_ERROR);

	if(m_pServerAddrInfoListForClient == 0)
	{
		struct addrinfo hints;

		bzero(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		if(getaddrinfo(m_strHostNameOrIP.c_str(), m_strServiceOrPort.c_str(), &hints, &m_pServerAddrInfoListForClient) != 0)
		{
			CLLogger::WriteLogMsg("In CLTCPClientSocket::Connect(), getaddrinfo error", errno);
			return CLStatus(-1, NORMAL_ERROR);
		}

		m_pCurrentAddrInfo = m_pServerAddrInfoListForClient;
	}

	if(m_pCurrentAddrInfo == 0)
	{
		freeaddrinfo(m_pServerAddrInfoListForClient);
		m_pServerAddrInfoListForClient = 0;

		return CLStatus(-1, TRY_CONNECT_END);
	}

	for(struct addrinfo *ptmp = m_pCurrentAddrInfo; ptmp != 0; ptmp = ptmp->ai_next)
	{
		int fd = socket(ptmp->ai_family, ptmp->ai_socktype, ptmp->ai_protocol);
		if(fd == -1)
		{
			CLLogger::WriteLogMsg("In CLTCPClientSocket::Connect(), socket error", errno);
			continue;
		}

		try
		{
			if(!InitialSocket(fd, m_bBlock).IsSuccess())
				throw CLStatus(0, 0);

			if(connect(fd, ptmp->ai_addr, ptmp->ai_addrlen) == 0)
			{
				m_SocketFd = fd;

				freeaddrinfo(m_pServerAddrInfoListForClient);
				
				m_pServerAddrInfoListForClient = 0;
				m_pCurrentAddrInfo = 0;

				return CLStatus(0, 0);
			}

			if(errno == EINPROGRESS)
			{
				m_SocketFd = fd;

				m_pCurrentAddrInfo = ptmp->ai_next;

				return CLStatus(-1, CONNECT_PENDING);
			}

			throw CLStatus(0, 0);
		}
		catch(CLStatus& s)
		{
			if(close(fd) == -1)
				CLLogger::WriteLogMsg("In CLTCPClientSocket::Connect(), close error", errno);
		}
	}

	freeaddrinfo(m_pServerAddrInfoListForClient);
	m_pServerAddrInfoListForClient = 0;
	m_pCurrentAddrInfo = 0;

	CLLogger::WriteLogMsg("In CLTCPClientSocket::Connect(), m_SocketFd == -1 error", 0);
	return CLStatus(-1, TRY_CONNECT_END);
}

void CLTCPClientSocket::NotifyConnectResults(bool bResults)
{
	if(bResults)
	{
		if(m_pServerAddrInfoListForClient)
			freeaddrinfo(m_pServerAddrInfoListForClient);

		m_pServerAddrInfoListForClient = 0;
		m_pCurrentAddrInfo = 0;
	}
	else
	{
		if(m_SocketFd != -1)
		{
			if(close(m_SocketFd) == -1)
				CLLogger::WriteLogMsg("In CLTCPClientSocket::NotifyConnectResults(), close error", errno);
			
			m_SocketFd = -1;
		}
	}
}