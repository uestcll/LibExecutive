#ifndef CLSocket_H
#define CLSocket_H

#include "CLUuid.h"
#include "DefinitionForConst.h"
#include "CLStatus.h"

class CLBaseSocket;
class CLIOVectors;
class CLSocketAddress;

class CLSocket
{
public:
	//server
	CLSocket(const char *pstrServiceOrPort, int SocketType = SOCKET_TYPE_TCP, bool bBlock = false, const char *pstrHostNameOrIP = 0, int backlog = LISTEN_BACKLOG);

	//connected socket
	CLSocket(int SocketFd, bool bBlock);

	//client
	CLSocket(const char *pstrHostNameOrIP, const char *pstrServiceOrPort, int SocketType = SOCKET_TYPE_TCP, bool bBlock = false);
	virtual ~CLSocket();

	int GetSocket();
	CLUuid GetUuid();

	CLStatus Accept(CLSocket **ppSocket);

	CLStatus Connect();
	void NotifyConnectResults(bool bResults);

	CLStatus Read(CLIOVectors& IOVectors, CLSocketAddress *pSocketAddress = 0);
	CLStatus Write(CLIOVectors& IOVectors, CLSocketAddress *pSocketAddress = 0);

private:
	CLSocket(const CLSocket&);
	CLSocket& operator=(const CLSocket&);

private:
	CLBaseSocket *m_pSocket;
	CLUuid m_UuidOfSocket;
};

#endif