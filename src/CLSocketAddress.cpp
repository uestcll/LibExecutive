#include <string.h>
#include "CLSocketAddress.h"
#include "CLLogger.h"

CLSocketAddress::CLSocketAddress()
{
	bzero(&m_addr, sizeof(struct sockaddr));
}

//other construct (has parameters)
CLSocketAddress::CLSocketAddress(struct sockaddr& addr)
{
	memcpy(&m_addr, &addr, sizeof(struct sockaddr));
}

CLSocketAddress::~CLSocketAddress()
{

}

struct sockaddr_in* CLSocketAddress::GetAddress()
{
	return (struct sockaddr_in*)(&m_addr);
}

socklen_t CLSocketAddress::GetAddressLength()
{
	return sizeof(m_addr);
}
