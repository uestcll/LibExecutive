#ifndef CLSocket_H
#define CLSocket_H

#include "DefinitionForConst.h"
#include "CLStatus.h"

class CLBaseSocket;
class CLIOVectors;
struct addrinfo;

class CLSocket
{
public:
	CLSocket(const char *pstrServiceOrPort, bool bBlock = false, const char *pstrHostNameOrIP = 0, int backlog = LISTEN_BACKLOG);
	CLSocket(int SocketFd, bool bBlock);
	CLSocket(const char *pstrHostNameOrIP, const char *pstrServiceOrPort, bool bBlock = false);
	virtual ~CLSocket();

	int GetSocket();

	CLStatus Accept(CLSocket **ppSocket);

	CLStatus Connect(const char *pstrHostNameOrIP, const char *pstrServiceOrPort);

	CLStatus Read(CLIOVectors& IOVectors, struct addrinfo *pAddrInfo = 0);
	CLStatus Write(CLIOVectors& IOVectors, struct addrinfo *pAddrInfo = 0);

private:
	CLSocket(const CLSocket&);
	CLSocket& operator=(const CLSocket&);

private:
	CLBaseSocket *m_pSocket;
};

#endif