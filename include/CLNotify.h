#ifndef CLNOTIFY_H
#define CLNOTIFY_H

#include "CLStatus.h"
#include "CLEvent.h"

//class CLEvent;
class CLEventFd;

#define EVENT 0
#define EVENTFD 1

class CLNotify
{
public:
    CLNotify(int type, bool isSemaphore = true, const char *name = NULL);
    ~CLNotify();
    CLStatus Set();
    CLStatus Wait();
    CLEvent* GetEvent();

private:
    int m_type;
    CLEvent *p_event;
    CLEventFd *p_eventFd;
};

#endif
