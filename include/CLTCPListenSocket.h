#ifndef CLTCPListenSocket_H
#define CLTCPListenSocket_H

#include "CLBaseSocket.h"

class CLSocket;

class CLTCPListenSocket : public CLBaseSocket
{
public:
	CLTCPListenSocket(const char *pstrHostNameOrIP, const char *pstrServiceOrPort, int backlog, bool bBlock);
	virtual ~CLTCPListenSocket();

	virtual CLStatus Accept(CLSocket **ppSocket);

private:
	CLTCPListenSocket(const CLTCPListenSocket&);
	CLTCPListenSocket& operator=(const CLTCPListenSocket&);
};

#endif