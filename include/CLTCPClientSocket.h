#ifndef CLTCPCLIENTSOCKET_H
#define CLTCPCLIENTSOCKET_H

#include "CLStatus.h"
#include "CLBaseSocket.h"

class CLSocket;

class CLTCPClientSocket : public CLBaseSocket
{
public:
	CLTCPListenSocket(const char *pHostNameOrIp, const char *pServiceNameOrPort, bool isBlock);
	virtual ~CLTCPClientSocket();

	CLStatus Connect();
	void  NotifyConnectResult(bool bResult)


private:
	char* m_pHostOrIP;
	char* m_pServiceOrPort;

	struct addrinfo *m_pAddressInfo;

private:
	CLTCPClientSocket(const CLTCPClientSocket&);
	CLTCPClientSocket& operator=(const CLTCPClientSocket&);
};

#endif