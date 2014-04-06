#ifndef CLMSGLOOPMANAGERFORLIBEVENT_H
#define CLMSGLOOPMANAGERFORLIBEVENT_H

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

    CLStatus RegisterReadEvent();
    CLstatus UnRegisterReadEvent();

    CLStatus RegisterWriteEvent();
    CLStatus UnRegisterWriteEvent();

    protected:
    
}
#endif
