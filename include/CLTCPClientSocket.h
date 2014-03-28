#ifndef CLTCPCLIENTSOCKET_H
#define CLTCPCLIENTSOCKET_H

#include <string>
#include "CLStatus.h"
#include "CLBaseSocket.h"

class CLSocket;

class CLTCPClientSocket : public CLBaseSocket
{
public:
	CLTCPClientSocket(const char *pHostNameOrIp, const char *pServiceNameOrPort, bool isBlock);
	virtual ~CLTCPClientSocket();

	CLStatus Connect();
	void  NotifyConnectResult(bool bResult);


private:
	std::string m_pHostOrIP;
	std::string m_pServiceOrPort;

	struct addrinfo *m_pAddressInfo;

private:
	CLTCPClientSocket(const CLTCPClientSocket&);
	CLTCPClientSocket& operator=(const CLTCPClientSocket&);
};

#endif