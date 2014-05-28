#include "CLNotify.h"
#include "CLEvent.h"
#include "CLEventFd.h"

CLNotify::CLNotify(int type)
{
    p_event = NULL;
    p_eventFd = NULL;
    m_type = type;
    if(m_type == EVENT)
        p_event = new CLEvent();
    if(m_type == EVENTFD)
        p_eventFd = new CLEventFd();
}

CLNotify::~CLNotify()
{
    if(NULL != p_event)
        delete p_event;
    if(NULL != p_eventFd)
        delete p_eventFd;
}

CLStatus CLNotify::Set()
{
    if(m_type == EVENT)
        p_event->Set();
    else
        m_peventFd->WriteFd();

    return CLStatus(0, 0);
}

CLStatus CLNotify::Wait()
{
    if(m_type == EVENT)
        p_event->Wait();
    else
        p_eventFd->ReadFd();
}
