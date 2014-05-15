#include <cstring>
#include <utility>
#include <vector>
#include "CLMsgLoopManagerForLibevent.h"
#include "CLCriticalSection.h"
#include "ErrorCode.h"
#include "CLMessageReceiver.h"
#include "CLLogger.h"

using namespace std;

CLMsgLoopManagerForLibevent::CLMsgLoopManagerForLibevent(CLMessageObserver *pMsgObserver, const char* pstrThreadName, bool bMultipleThread) : CLMessageLoopManager(pMsgObserver)
{
	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLMsgLoopManagerForIOMultiplexing::CLMsgLoopManagerForIOMultiplexing(), pstrThreadName error";

	m_strThreadName = pstrThreadName;

    m_bMultipleThread = bMultipleThread;
}

CLMsgLoopManagerForLibevent::~CLMsgLoopManagerForLibevent()
{
	map<CLLibEvent, CLMessageReceiver*>::iterator it = m_ReadSetMap.begin();
    for(; it != m_ReadSetMap.end(); ++it)
    {
        delete it->second;
    }
}

CLStatus CLMsgLoopManagerForLibevent::Internal_RegisterReadEvent(CLLibEvent ev, CLMessageReceiver *pMsgReceiver)
{
    map<CLLibEvent, CLMessageReceiver*>::iterator it = m_ReadSetMap.find(ev);
    if(it != m_ReadSetMap.end())
        return CLStatus(-1, NORMAL_ERROR);

    m_ReadSetMap[ev] = pMsgReceiver;

    return CLStatus(0, 0);
}
CLStatus CLMsgLoopManagerForLibevent::RegisterReadEvent(CLLibEvent ev, CLMessageReceiver *pMsgReceiver)
{
    if(pMsgReceiver == 0)
        return CLStatus(-1, NORMAL_ERROR);
    
    if(m_bMultipleThread)
    {
        CLCriticalSection cs(&m_MutexForReadMap);

        if(Internal_RegisterReadEvent(ev, pMsgReceiver).IsSuccess())
            return CLStatus(0, 0);
    }
    else
    {
        if(Internal_RegisterReadEvent(ev, pMsgReceiver).IsSuccess())
            return CLStatus(0, 0);
    }

    CLLogger::WriteLogMsg("In CLMsgLoopManagerForLibevent::RegisterReadEvent(), Internal_RegisterReadEvent error", -1);

    delete pMsgReceiver;

    return CLStatus(-1, NORMAL_ERROR);
}

CLStatus CLMsgLoopManagerForLibevent::UnRegisterReadEvent(CLLibEvent ev)
{
    if(m_bMultipleThread)
    {
        {
            CLCriticalSection cs(&m_MutexForDeletedSet);

            set<CLLibEvent>::iterator it = m_DeletedSet.find(ev);
            if(it != m_DeletedSet.end())
                return CLStatus(0, 0);

            pair<set<CLLibEvent>::iterator, bool> r = m_DeletedSet.insert(ev);

            if(r.second)
                return CLStatus(0, 0);
        }

        CLLogger::WriteLogMsg("In CLMsgLoopManagerForLibevent::UnRegisterReadEvent(), m_DeletedSet.insert error", -1);

        return CLStatus(-1, NORMAL_ERROR);
    }
    else
    {
        CLStatus s = Internal_UnRegisterReadEvent(ev);
        if(!s.IsSuccess())
        {
            CLLogger::WriteLogMsg("In CLMsgLoopManagerForLibevent::UnRegisterReadEvent(), Internal_UnRegisterReadEvent error", -1);
            return s;
        }

        CLMessageReceiver *pTmp = (CLMessageReceiver*)s.m_clErrorCode;

        if(pTmp)
            delete pTmp;

        return s;
    }
}

CLStatus CLMsgLoopManagerForLibevent::Internal_UnRegisterReadEvent(CLLibEvent ev)
{
    map<CLLibEvent, CLMessageReceiver*>::iterator it = m_ReadSetMap.find(ev);
    if(it == m_ReadSetMap.end())
        return CLStatus(-1, NORMAL_ERROR);

    CLMessageReceiver *pTmp = it->second;

    m_ReadSetMap.erase(it);

    return CLStatus(0, (long)pTmp);
}

void CLMsgLoopManagerForLibevent::ClearDeletedSet()
{
	vector<CLMessageReceiver *> vpMsgReceiver;
	
	{
		CLCriticalSection cs(&m_MutexForDeletedSet);

		set<CLLibEvent>::iterator iter = m_DeletedSet.begin();
        for(; iter != m_DeletedSet.end(); ++iter)
        {
            long r, e;

            {
                CLCriticalSection cs1(&m_MutexForReadMap);

                CLStatus s = Internal_UnRegisterReadEvent(*iter);
                r = s.m_clReturnCode;
                e = s.m_clErrorCode;
            }

            CLStatus s1(r, e);
            if(!s1.IsSuccess())
            {
                CLLogger::WriteLogMsg("In CLMsgLoopManagerForLibevent::ClearDeletedSet, Internal_UnRegisterReadEvent error", - 1);
                continue;
            }

            CLMessageReceiver *pTmp = (CLMessageReceiver*)s1.m_clErrorCode;
            if(pTmp)
                vpMsgReceiver.push_back(pTmp);
        }

        m_DeletedSet.clear();
    }

    vector<CLMessageReceiver*>::iterator it = vpMsgReceiver.begin();
    for(; it != vpMsgReceiver.end(); ++it)
    {
        delete (*it);
    }
}

CLStatus CLMsgLoopManagerForLibevent::RegisterWriteEvent(CLLibEvent ev, CLMessagePoster *pMsgPoster)
{
    if(m_bMultipleThread)
    {
        CLCriticalSection cs(&m_MutexForWriteMap);

        return Internal_RegisterWriteEvent(ev, pMsgPoster);
    }
    else
    {
        return Internal_RegisterWriteEvent(ev, pMsgPoster);
    }
}

CLStatus CLMsgLoopManagerForLibevent::Internal_RegisterWriteEvent(CLLibEvent ev, CLMessagePoster *pMsgPoster)
{
    map<CLLibEvent, CLMessagePoster*>::iterator it = m_WriteSetMap.find(ev);
    if(it != m_WriteSetMap.end())
        return CLStatus(0, 0);

    m_WriteSetMap[ev] = pMsgPoster;


    return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForLibevent::Internal_RegisterConnectEvent(CLLibEvent ev, CLDataPostChannelMainter *pChannel)
{
    map<CLLibEvent, CLDataPostChannelMainter*>::iterator it = m_ChannelMap.find(ev);
    if(it != m_ChannelMap.end())
        return CLStatus(0, 0);

    m_ChannelMap[ev] = pChannel;

    Internal_RegisterReadEvent(ev, 0);

    Internal_RegisterWriteEvent(ev, 0);

    return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForLibevent::RegisterConnectEvent(CLLibEvent ev, CLDataPostChannelMainter *pChannel)
{
    if(pChannel == 0)
        return CLStatus(-1, NORMAL_ERROR);

    if(m_bMultipleThread)
    {
        CLCriticalSection cs(&m_MutexForChannelMap);

        {
            CLCriticalSection cs1(&m_MutexForReadMap);

            {
                CLCriticalSection cs2(&m_MutexForWriteMap);

                return Internal_RegisterConnectEvent(ev, pChannel);
            }
        }
    }
    else
    {
        return Internal_RegisterConnectEvent(ev, pChannel);
    }
}

CLStatus CLMsgLoopManagerForLibevent::Initialize()
{
    return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForLibevent::Uninitialize()
{
    return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForLibevent::WaitForMessage()
{
    if(m_bMultipleThread)
        ClearDeletedSet();
}
