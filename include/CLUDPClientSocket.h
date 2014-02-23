#ifndef CLUDPClientSocket_H
#define CLUDPClientSocket_H

#include <string>
#include "CLBaseSocket.h"

struct addrinfo;

class CLUDPClientSocket : public CLBaseSocket
{
public:
	CLUDPClientSocket();
	virtual ~CLUDPClientSocket();

private:
	CLUDPClientSocket(const CLUDPClientSocket&);
	CLUDPClientSocket& operator=(const CLUDPClientSocket&);
};

#endif