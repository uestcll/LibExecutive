#ifndef CLUDPServerSocket_H
#define CLUDPServerSocket_H

#include "CLBaseSocket.h"

class CLUDPServerSocket : public CLBaseSocket
{
public:
	CLUDPServerSocket(const char *pstrHostNameOrIP, const char *pstrServiceOrPort, bool bBlock);
	virtual ~CLUDPServerSocket();

private:
	CLUDPServerSocket(const CLUDPServerSocket&);
	CLUDPServerSocket& operator=(const CLUDPServerSocket&);
};

#endif