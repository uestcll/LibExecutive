#ifndef CLEVENTFD_H
#define CLEVENTFD_H

#include <sys/eventfd.h>

class CLEventFd
{
public:
    CLEventFd();
    ~CLEventFd();
    int GetFd();
    int WriteFd();
    int WriteFd(long n);
    int ReadFd();
    int ReadFd(long n);

private:
    int m_fd;
};

#endif
