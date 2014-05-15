#ifndef CLMSGLOOPMANAGERFORLIBEVENT_H
#define CLMSGLOOPMANAGERFORLIBEVENT_H

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event.h>
#include <list>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <map>
#include <set>
#include "CLMessageLoopManager.h"
#include "CLMutex.h"

class CLMessageReceiver;
class CLMessagePoster;
class CLDataPostChannelMainter;

class CLLibEvent
{
public:
    CLLibEvent() : m(0)
    {
    }
    ~CLLibEvent()
    {
    }

    bool operator<(const CLLibEvent &ev) const
    {
        return m < ev.m;
    }

private:
    struct event ev;
    int m;
};

class CLMsgLoopManagerForLibevent : public CLMessageLoopManager
{
public:
    CLMsgLoopManagerForLibevent(CLMessageObserver *pMsgObserver, const char *pstrThreadName, bool m_bMultipleThread);
    virtual ~CLMsgLoopManagerForLibevent();

    CLStatus RegisterReadEvent(CLLibEvent ev, CLMessageReceiver *pMsgReceiver);
    CLStatus UnRegisterReadEvent(CLLibEvent ev);

    CLStatus RegisterWriteEvent(CLLibEvent ev, CLMessagePoster *pMsgPoster);
    CLStatus RegisterConnectEvent(CLLibEvent ev, CLDataPostChannelMainter *pChannel);

protected:
    virtual CLStatus Initialize();
    virtual CLStatus Uninitialize();

    virtual CLStatus WaitForMessage();

private:
    void ClearDeletedSet();

    CLStatus Internal_RegisterReadEvent(CLLibEvent ev, CLMessageReceiver *pMsgReceiver);
    CLStatus Internal_UnRegisterReadEvent(CLLibEvent ev);

    CLStatus Internal_RegisterWriteEvent(CLLibEvent ev, CLMessagePoster *pMsgPoster);

    CLStatus Internal_RegisterConnectEvent(CLLibEvent ev, CLDataPostChannelMainter *pChannel);

private:
    CLMsgLoopManagerForLibevent(const CLMsgLoopManagerForLibevent&);
    CLMsgLoopManagerForLibevent& operator=(const CLMsgLoopManagerForLibevent&);

private:
    std::string m_strThreadName;

    std::map<CLLibEvent, CLMessageReceiver*> m_ReadSetMap;
    std::map<CLLibEvent, CLMessagePoster*> m_WriteSetMap;
    std::map<CLLibEvent, CLDataPostChannelMainter*> m_ChannelMap;
    std::set<CLLibEvent> m_DeletedSet;

    CLMutex m_MutexForReadMap;
    CLMutex m_MutexForWriteMap;
    CLMutex m_MutexForDeletedSet;  
    CLMutex m_MutexForChannelMap;

    bool m_bMultipleThread;
};

#endif
