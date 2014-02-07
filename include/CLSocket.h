#ifndef CLSocket_H
#define CLSocket_H

#include <sys/types.h>
#include <sys/socket.h>
#include <vector>
#include "DefinitionForConst.h"
#include "CLStatus.h"

class CLIOVectors;
struct addrinfo;

class CLSocket
{
public:
	CLSocket(const char *pstrServiceOrPort, bool bBlock = false, const char *pstrHostNameOrIP = 0, int backlog = LISTEN_BACKLOG);
	virtual ~CLSocket();

	int GetSocket();

	CLStatus Read(CLIOVectors& IOVectors);
	CLStatus Write(CLIOVectors& IOVectors, struct addrinfo *pAddrInfo = 0);

private:
	CLSocket(const CLSocket&);
	CLSocket& operator=(const CLSocket&);

private:
	int m_SocketFd;
	bool m_bBlock;
};

#endif