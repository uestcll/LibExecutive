#ifndef CLUDPClientSocket_H
#define CLUDPClientSocket_H

#include <string>
#include "CLBaseSocket.h"

struct addrinfo;

class CLUDPClientSocket : public CLBaseSocket
{
public:
	CLUDPClientSocket(const char *pstrHostNameOrIP, const char *pstrServiceOrPort, bool bBlock);
	virtual ~CLUDPClientSocket();

private:
	CLUDPClientSocket(const CLUDPClientSocket&);
	CLUDPClientSocket& operator=(const CLUDPClientSocket&);
};

#endif