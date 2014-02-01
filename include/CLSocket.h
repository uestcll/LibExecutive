#ifndef CLSocket_H
#define CLSocket_H

#include <sys/types.h>
#include <sys/socket.h>
#include "CLStatus.h"

class CLSocket
{
public:
	CLSocket(bool bBlock = false, int family = AF_INET, int type = SOCK_STREAM);
	virtual ~CLSocket();

	int GetSocket();
	CLStatus InitialServer(int port, int backlog, const char *strBoundIP = 0);

private:
	CLSocket(const CLSocket&);
	CLSocket& operator=(const CLSocket&);

private:
	int m_SocketFd;
	int m_Family;
};

#endif