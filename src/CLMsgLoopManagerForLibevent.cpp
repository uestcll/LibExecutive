/*************************************************************************
	> File Name: CLMsgLoopManagerForLibevent.cpp
	> Author: lemon 
	> Mail: lemon_wonder@163.com 
	> Created Time: Mon 24 Mar 2014 11:14:14 PM CST
 ************************************************************************/
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


CLMsgLoopManagerForLibevent::CLMsgLoopManagerForLibevent(CLMessageObserver *pMsgObserver, const char *pstrThreadName) : CLMessageLoopManager(pMsgObserver)
{
    if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
    {
        throw "In CLMsgLoopManagerForLibevent::CLMsgLoopManagerForLibevent(), pstrThreadName error";
    }

    m_strThreadName = pstrThreadName;
    
}
