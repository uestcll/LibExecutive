#ifndef CLNOTIFY_H
#define CLNOTIFY_H

#include "CLStatus.h"

class CLEvent;
class CLEventFd;

#define EVENT 0
#define EVENTFD 1

class CLNotify
{
public:
    CLNotify(int type);
    ~CLNotify();
    CLStatus Set();
    CLStatus Wait();

private:
    int m_type;
    CLEvent *p_event;
    CLEventFd *p_eventFd;
};

#endif
