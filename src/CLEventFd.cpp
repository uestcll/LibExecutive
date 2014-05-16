#include "CLEventFd.h"
#include <unistd.h>

CLEventFd::CLEventFd()
{
    m_fd = eventfd(0, 0);
}

CLEventFd::~CLEventFd()
{
    if(m_fd >= 0)
        close(m_fd);
}

int CLEventFd::GetFd()
{
    return m_fd;
}

int CLEventFd::WriteFd()
{
    long n;
    return write(m_fd, n, sizeof(long));
}

int CLEventFd::WriteFd(long n)
{
    return write(m_fd, n, sizeof(long));
}

int CLEventFd::ReadFd()
{
    long n;
    return read(m_fd, n, sizeof(long));
}

int CLEventFd::ReadFd(long n)
{
    return read(m_fd, n, sizeof(long));
}
