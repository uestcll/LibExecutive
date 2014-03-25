#ifndef CLMSGLOOPMANAGERFORLIBEVENT_H
#define CLMSGLOOPMANAGERFORLIBEVENT_H

#include "CLMessageLoopManager.h"

class CLMessageReceiver;
class CLBufferManager;
class CLProtocolEncapsulator;
class CLProtocolDecapsulator;
class CLMessageDeserializer;
class CLMessageSerializer;

class CLMsgLoopManagerForLibevent : public CLMessageLoopManager
{
    public:
    CLMsgLoopManagerForLibevent(CLMessageObserver *pMsgObserver, const char *pstrThreadName);
    virtual ~CLMsgLoopManagerForLibevent();

    protected:
    virtual CLStatus Initialize();
    virtual CLStatus Uninitialize();

    virtual CLStatus WaitForMessage();

    pritvate:
    CLMsgLoopManagerForLibevent(const CLMsgLoopManagerForLibevent&);
    CLMsgLoopManagerForLibevent& operator=(const CLMsgLoopManagerForLibevent&);

    private:
    std::string m_strThreadName;
    CLMessageReceiver *m_pMsgReceiver;
    CLEvent *m_pEvent;
    
};

#endif
