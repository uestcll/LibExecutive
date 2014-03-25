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
    }
}
