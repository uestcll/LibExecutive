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

		if(getaddrinfo(m_strHostNameOrIP.c_str(), m_pstrServiceOrPort.c_str(), &hints, &m_pServerAddrInfoListForClient) != 0)
		{
			CLLogger::WriteLogMsg("In CLTCPClientSocket::Connect(), getaddrinfo error", errno);
			return CLStatus(-1, NORMAL_ERROR);
		}

		m_pCurrentAddrInfo = m_pServerAddrInfoListForClient;
	}

	//.......................
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
}