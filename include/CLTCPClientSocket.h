#ifndef CLTCPClientSocket_H
#define CLTCPClientSocket_H

#include <string>
#include "CLBaseSocket.h"

struct addrinfo;

class CLTCPClientSocket : public CLBaseSocket
{
public:
	CLTCPClientSocket(const char *pstrHostNameOrIP, const char *pstrServiceOrPort, bool bBlock);
	virtual ~CLTCPClientSocket();

	virtual CLStatus Connect();

private:
	CLTCPClientSocket(const CLTCPClientSocket&);
	CLTCPClientSocket& operator=(const CLTCPClientSocket&);

private:
	std::string m_strHostNameOrIP;
	std::string m_strServiceOrPort;

	struct addrinfo *m_pServerAddrInfoListForClient;
	struct addrinfo *m_pCurrentAddrInfo;
};

#endif