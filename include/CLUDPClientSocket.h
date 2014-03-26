#ifndef CLUCPCLIENTSOCKET_H
#define CLUCPCLIENTSOCKET_H

#include "CLStatus.h"
#include "CLBaseSocket.h"

class CLSocket;

class CLUCPClientSocket : public CLBaseSocket
{
public:
	CLUCPClientSocket(const char *pHostNameOrIp, const char *pServiceNameOrPort, bool isBlock);
	virtual ~CLUCPClientSocket();

private:
	CLUCPClientSocket(const CLUCPClientSocket&);
	CLUCPClientSocket& operator=(const CLUCPClientSocket&);
};

#endif