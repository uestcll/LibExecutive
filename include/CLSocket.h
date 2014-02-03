#ifndef CLSocket_H
#define CLSocket_H

#include <sys/types.h>
#include <sys/socket.h>
#include <vector>
#include "DefinitionForConst.h"
#include "CLStatus.h"

class CLSocket
{
public:
	CLSocket(const char *pstrServiceOrPort, bool bBlock = false, const char *pstrHostNameOrIP = 0, int backlog = LISTEN_BACKLOG);
	virtual ~CLSocket();

	int GetSocket();

private:
	CLSocket(const CLSocket&);
	CLSocket& operator=(const CLSocket&);

private:
	int m_SocketFd;
};

#endif