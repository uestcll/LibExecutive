#include <string.h>
#include "CLMsgLoopManagerForLibevent.h"
#include "CLMessageReceiver.h"
#include "CLPointerToMsgDeserializer.h"
#include "CLExecutiveNameServer.h"
#include "CLDataReceiverByLibevent.h"
#include "CLLogger.h"
#include "CLEvent.h"
#include "ErrorCode.h"
#include "CLBufferManager.h"
#include "CLMessagePoster.h"
#include "CLMsgToPointerSerializer.h"
#include "CLDataPostChannelByLibeventMaintainer.h"
#include "CLProtocolDecapsulatorBySplitPointer.h"
#include "CLInitialDataPostChannelNotifier.h"

CLMsgLoopManagerForLibevent::CLMsgLoopManagerForLibevent(CLMessageObserver *pMsgObserver, const char *pstrThreadName) : CLMessageLoopManager(pMsgObserver)
{
    if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
    {
        throw "In CLMsgLoopManagerForLibevent::CLMsgLoopManagerForLibevent(), pstrThreadName error";
    }

    m_strThreadName = pstrThreadName;
    m_pEvent = new CLEvent(true);
    m_pMsgReceiver = new CLMessageReceiver(new CLBufferManager(), new CLDataReceiverByLibevent(), new CLPointerToMsgDeserializer(), new CLProtocolDecapsulatorBySplitPointer);
    
}

CLMsgLoopManagerForLibevent::~CLMsgLoopManagerForLibevent()
{}

CLstatus CLMsgLoopManagerForLibevent::Initialize()
{
    if(m_pMsgReceiver == 0)
    {
        return CLStatus(-1, 0);
    }

    CLMessagePoster *pMsgPoster = 0;

    try
    {
        CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
        if(pNameServer == 0)
        {
            CLLogger::WriteLogMsg("In CLMsgLoopManagerForLibevent::Initialize(), CLExecutiveNameServer::GetInstance error", 0);
            throw CLStatus(-1, 0);
        }
        pMsgPoster = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByLibeventMaintainer(), m_pEvent);

        CLStatus s2 = pMsgPoster->Initialize(new CLInitialDataPostChannelNotifier(), 0);
        if(!s2.IsSuccess() && (s2.clErrorCode == DATA_POSTER_INITIALIZE_ERROR))
        {
            CLLogger::WriteLogMsg("In CLMsgLoopManagerForLibevent::Initialize(), pMsgPoster->Initialize error", 0);
            throw CLStatus(-1, 0);
        }

        CLStatus s = pNameServer->Register(m_strThreadName.c_str(), pMsgPoster, m_pMsgReceiver);

        if(!s.IsSuccess())
        {
            CLLogger::WriteLogMsg("In CLMsgLoopManagerForLibevent::Initialize(), pNameServer->Register error", 0);

            m_pMsgReceiver = 0;
            return CLStatus(-1, 0);
        }

        return CLStatus(0, 0);
    }
    catch(CLStatus& s1)
    {
        if(m_pMsgReceiver)
        {
            delete m_pMsgReceiver;
            m_pMsgReceiver = 0;
        }

        if(pMsgPoster)
            delete pMsgPoster;
        else
        {
            delete m_pEvent;
        }

        return s1;
    }
}

CLStatus CLMsgLoopManagerForLibevent::Uninitialize()
{
    delete m_pMsgReceiver;
    m_pMsgReceiver = 0;

    CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
    if(pNameServer == 0)
    {
        CLLogger::WriteLogMsg("In CLMsgLoopManagerForLibevent::Uninitialize(), CLExecutiveNameServer::GetInstance error", 0);
        return CLstatus(-1, 0);
    }

    return pNameServer->ReleaseCommunicationPtr(m_strThreadName.c_str());
}

CLstatus CLMsgLoopManagerForLibevent::WaitForMessage()
{
    CLStatus s = m_pEvent->Wait();
    if(!s.IsSuccess())
    {
        CLLogger::WriteLogMsg("In CLMsgLoopManagerForLibevent::WaitForMessage(), m_Event.Wait error", 0);
        return CLstatus(-1, 0);
    }

    long old_size = m_MessageContainer.size();

    CLStatus s1 = m_pMsgReceiver->GetMessage(m_MessageContainer);
    
    long new_size = m_MessageContainer.size();

    if(new_size > old_size)
    {
        if(!(m_pEvent->ReleaseSemaphore(new_size - old_size - 1).IsSuccess()))
        {
            CLLogger::WriteLogMsg("In CLMsgLoopManagerForLibevent::WaitForMessage(), m_pEvent->ReleaseSemaphore error; but may be made by the sequence of sendmsg.set(wait)sendmsg(compute new_size).set", 0);
        }
    }

    return s1;
}
