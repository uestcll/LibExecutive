#ifndef CLTCPLISTENSOCKET_H
#define CLTCPLISTENSOCKET_H

#include "CLStatus.h"
#include "CLBaseSocket.h"

class CLSocket;

class CLTCPListenSocket : public CLBaseSocket
{
public:
	CLTCPListenSocket(const char *pHostNameOrIp, const char *pServiceNameOrPort, bool isBlock, int listenNum);
	virtual ~CLTCPListenSocket();

	CLStatus Accept(CLSocket **ppConnSock, CLSocketAddress **ppOppoAddr);

private:
	CLTCPListenSocket(const CLTCPListenSocket&);
	CLTCPListenSocket& operator=(const CLTCPListenSocket&);
};

#endif