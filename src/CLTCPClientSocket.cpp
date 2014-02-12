#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
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

	try
	{
		m_SocketFd = socket(m_pCurrentAddrInfo->ai_family, m_pCurrentAddrInfo->ai_socktype, m_pCurrentAddrInfo->ai_protocol);
		if(m_SocketFd == -1)
		{
			CLLogger::WriteLogMsg("In CLTCPClientSocket::Connect(), socket error", errno);
			throw CLStatus(-1, NORMAL_ERROR);
		}

		if(!InitialSocket(m_SocketFd, m_bBlock).IsSuccess())
		{
			if(close(m_SocketFd) == -1)
				CLLogger::WriteLogMsg("In CLTCPClientSocket::Connect(), close error", errno);

			m_SocketFd = -1;

			throw CLStatus(-1, NORMAL_ERROR);
		}


	}
	catch(CLStatus& s)
	{
		m_pCurrentAddrInfo = m_pCurrentAddrInfo->ai_next;
		return s;
	}
}