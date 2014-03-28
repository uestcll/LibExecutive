#ifndef CLSOCKETADDRESS_H
#define CLSOCKETADDRESS_H

#include <sys/types.h>
#include <sys/socket.h>

class CLSocketAddress
{
public:
	CLSocketAddress();
	explicit CLSocketAddress(struct sockaddr& addr);
	virtual ~CLSocketAddress();

	struct sockaddr* GetAddress();
	socklen_t GetAddressLength();

private:
	struct sockaddr  m_addr;

private:
	CLSocketAddress(const CLSocketAddress&);
	CLSocketAddress& operator=(const CLSocketAddress&);
};

#endif