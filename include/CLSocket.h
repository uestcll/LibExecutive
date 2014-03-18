#ifndef CLSOCKET_H
#define CLSOCKET_H

#include "CLStatus.h"

class CLBaseSocket;
class CLIOVector;
class CLSocketAddress;

#define TCP_SOCKET 1
#define UDP_SOCKET 2

class CLSocket
{
public:
	CLSocket(const char *pHostNameOrIp, const char *pServiceNameOrPort, bool forServer = true, int sockeType = TCP_SOCKET, bool isBlock = true, int listenNum = MAX_LISTEN_NUM);
	CLSocket(int sockFd, bool isBlock);
	virtual ~CLSocket();

	CLStatus Accept(CLSocket **ppConnSock);
	CLStatus Connect();

	CLStatus Read(CLIOVector& IOVec, CLSocketAddress* pAddr = 0);
	CLStatus Write(CLIOVector& IOVec, CLSocketAddress *pAddr = 0);

	const int GetSocket() const ;

	void  NotifyConnectResult(bool bResult);

private:
	CLBaseSocket *m_pSocket;

private:
	CLSocket(const CLSocket &);
	CLSocket& operator=(const CLSocket&);
};


#endif