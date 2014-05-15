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

class CLMsgLoopManagerForLibevent : public CLMessageLoopManager
{
public:
    CLMsgLoopManagerForLibevent(CLMessageObserver *pMsgObserver, const char *pstrThreadName, bool m_bMultipleThread);
    virtual ~CLMsgLoopManagerForLibevent();

    CLStatus RegisterReadEvent(struct event* ev, CLMessageReceiver *pMsgReceiver);
    CLStatus UnRegisterReadEvent(struct event* ev);

    CLStatus RegisterWriteEvent(struct event* ev, CLMessagePoster *pMsgPoster);
    CLStatus RegisterConnectEvent(struct event* ev, CLDataPostChannelMainter *pChannel);

protected:
    virtual CLStatus Initialize();
    virtual CLStatus Uninitialize();

    virtual CLStatus WaitForMessage();

private:
    void ClearDeletedSet();

    CLStatus Internal_RegisterReadEvent(struct event* ev, CLMessageReceiver *pMsgReceiver);
    CLStatus Internal_UnRegisterReadEvent(struct event* ev);

    CLStatus Internal_RegisterWriteEvent(struct event* ev, CLMessagePoster *pMsgPoster);

    CLStatus Internal_RegisterConnectEvent(struct event* ev, CLDataPostChannelMainter *pChannel);

private:
    CLMsgLoopManagerForLibevent(const CLMsgLoopManagerForLibevent&);
    CLMsgLoopManagerForLibevent& operator=(const CLMsgLoopManagerForLibevent&);

private:
    std::string m_strThreadName;

    std::map<struct event*, CLMessageReceiver*> m_ReadSetMap;
    std::map<struct event*, CLMessagePoster*> m_WriteSetMap;
    std::map<struct event*, CLDataPostChannelMainter*> m_ChannelMap;
    std::set<struct event*> m_DeletedSet;

    CLMutex m_MutexForReadMap;
    CLMutex m_MutexForWriteMap;
    CLMutex m_MutexForDeletedSet;  
    CLMutex m_MutexForChannelMap;

    bool m_bMultipleThread;
};

#endif
