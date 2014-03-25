#include <string.h>
#include "CLMsgLoopManagerForLibevent.h"
#include "CLMessageReceiver.h"
#include "CLPointToMsgDeserializer.h"
#include "CLExecutiveNameServer.h"
#include "CLLogger.h"
#include "CLEvent.h"
#include "ErrorCode.h"
#include "CLBufferManager.h"
#include "CLMessagePoster.h"
#include "CLMsgToPointerSerializer.h"
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
        pMsgObserver = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByLibeventMaintainer(), m_pEvent);

        CLStatus s2 = pMsgObserver->Initialize(new CLInitialDataPostChannelNotifier(), 0);
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
    m_pMsgReceiver = 0;
    CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
    if(pNameServer == 0)
    {
        CLLogger::WriteLogMsg("In CLMsgLoopManagerForLibevent::Uninitialize(), CLExecutiveNameServer::GetInstance error", 0);
        return CLstatus(-1, 0);
    }

    return pNameServer->ReleaseCommunicationPtr(m_strThreadName.c_str());
}
