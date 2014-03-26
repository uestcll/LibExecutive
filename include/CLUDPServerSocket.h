#ifndef CLUDPServerSOCKET_H
#define CLUDPServerSOCKET_H

#include "CLStatus.h"
#include "CLBaseSocket.h"

class CLUDPServerSocket : public CLBaseSocket
{
public:
	CLUDPServerSocket(const char *pHostNameOrIp, const char *pServiceNameOrPort, bool isBlock);
	virtual ~CLUDPServerSocket();

private:
	CLUDPServerSocket(const CLUDPServerSocket&);
	CLUDPServerSocket& operator=(const CLUDPServerSocket&);
};

#endif