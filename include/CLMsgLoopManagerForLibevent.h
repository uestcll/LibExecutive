#ifndef CLMSGLOOPMANAGERFORLIBEVENT_H
#define CLMSGLOOPMANAGERFORLIBEVENT_H

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <string>
#include <map>
#include <set>
#include <list>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "CLMessageLoopManager.h"
#include "CLMutex.h"

class CLMessageReceiver;
class CLProtocolDataPoster;

class CLMsgLoopManagerForLibevent : public CLMessageLoopManager
{
    public:
    CLMsgLoopManagerForLibevent(CLMesssageObserver *pMsgObserver, const char *pstrThreadName);
    virtual ~CLMsgLoopManagerForLibevent();

    CLStatus RegisterReadEvent(struct event ev, CLMessageReceiver *pMsgReceiver);
    CLstatus UnRegisterReadEvent(struct event ev);

    CLStatus RegisterWriteEvent(struct event ev, CLMessagePoster *pMsgPoster);
    CLStatus UnRegisterWriteEvent(struct event ev);

    protected:
    virtual CLStatus Initialize();
    virtual CLStatus Uninitialize();

    virtual CLStatus WaitForMessage();

    private:
    void ClearDeletedSet();

    private:
    CLMsgLoopManagerForLibevent(const CLMsgLoopManagerForLibevent&);
    CLMsgLoopManagerForLibevent& operator=(const CLMsgLoopManagerForLibevent&);

    private:
    std::string m_strThreadName;

    std::map<struct event, CLMessageReceiver*> m_ReadSetMap;
    std::map<struct event, std::list<CLMessagePoster*>*> m_WriteSetMap;

    std::set<struct event> m_DeletedSet;

    CLMutex m_MutexForReadMap;
    CLMutex m_MutexForWriteMap;
    CLMutex m_MutexForDeletedSet;  
};

#endif
