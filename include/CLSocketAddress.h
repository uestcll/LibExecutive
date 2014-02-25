#ifndef CLSocketAddress_H
#define CLSocketAddress_H

#include <sys/types.h>
#include <sys/socket.h>

class CLSocketAddress
{
public:
	CLSocketAddress();
	virtual ~CLSocketAddress();

	sockaddr* GetAddress();
	socklen_t GetAddressLength();

private:
	CLSocketAddress(const CLSocketAddress&);
	CLSocketAddress& operator=(const CLSocketAddress&);

private:
	sockaddr_storage m_SocketAddress;
};

#endif