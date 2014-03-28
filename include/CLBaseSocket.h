#ifndef CLBASESOCKET_H
#define CLBASESOCKET_H

#include "CLStatus.h"

class CLIOVector;
class CLSocketAddress;

class CLBaseSocket
{
public:
	CLBaseSocket(bool isBlock);
	CLBaseSocket(int fd, bool isBlock);
	virtual ~CLBaseSocket();

	CLStatus InitSocketFd(int fd, bool isBlock);

	CLStatus Read(CLIOVector& IOVec, CLSocketAddress *pAddr);
	CLStatus Write(CLIOVector& IOVec, CLSocketAddress *pAddr);

	const int GetSocketFd() const;

private:
	CLStatus ReadOrWrite(CLIOVector& IOVec, CLSocketAddress *pAddr, bool bWrite);

protected:
	int m_SocketFd;
	bool m_bBlock;

private:
	CLBaseSocket(const CLBaseSocket&);
	CLBaseSocket& operator=(const CLBaseSocket&);
};


#endif