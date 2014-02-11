#ifndef CLBaseSocket_H
#define CLBaseSocket_H

#include "CLStatus.h"

class CLIOVectors;
struct addrinfo;

class CLBaseSocket
{
public:
	explicit CLBaseSocket(bool bBlock);
	CLBaseSocket(int SocketFd, bool bBlock);
	virtual ~CLBaseSocket();

	virtual int GetSocket();

	virtual CLStatus Read(CLIOVectors& IOVectors, struct addrinfo *pAddrInfo);
	virtual CLStatus Write(CLIOVectors& IOVectors, struct addrinfo *pAddrInfo);

protected:
	CLStatus ReadOrWrite(bool bWrite, CLIOVectors& IOVectors, struct addrinfo *pAddrInfo);
	CLStatus InitialSocket(int SocketFd, bool bBlock);

private:
	CLBaseSocket(const CLBaseSocket&);
	CLBaseSocket& operator=(const CLBaseSocket&);

protected:
	int m_SocketFd;
	bool m_bBlock;
};

#endif