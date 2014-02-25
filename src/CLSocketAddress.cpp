#include <strings.h>
#include "CLSocketAddress.h"

CLSocketAddress::CLSocketAddress()
{
	bzero(&m_SocketAddress, sizeof(m_SocketAddress));
}

CLSocketAddress::~CLSocketAddress()
{
}

sockaddr* CLSocketAddress::GetAddress()
{
	return (sockaddr* )(&m_SocketAddress);
}

socklen_t CLSocketAddress::GetAddressLength()
{
	return sizeof(m_SocketAddress);
}