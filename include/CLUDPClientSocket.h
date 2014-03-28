#ifndef CLUDPCLIENTSOCKET_H
#define CLUDPCLIENTSOCKET_H

#include "CLStatus.h"
#include "CLBaseSocket.h"

class CLSocket;

class CLUDPClientSocket : public CLBaseSocket
{
public:
	CLUDPClientSocket(const char *pHostNameOrIp, const char *pServiceNameOrPort, bool isBlock);
	virtual ~CLUDPClientSocket();

private:
	CLUDPClientSocket(const CLUDPClientSocket&);
	CLUDPClientSocket& operator=(const CLUDPClientSocket&);
};

#endif