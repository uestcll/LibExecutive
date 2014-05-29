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

    m_base = event_base_new();
}

CLMsgLoopManagerForLibevent::~CLMsgLoopManagerForLibevent()
{
	map<struct event*, CLMessageReceiver*>::iterator it = m_ReadSetMap.begin();
    for(; it != m_ReadSetMap.end(); ++it)
    {
        delete it->second;
    }

}

CLStatus CLMsgLoopManagerForLibevent::Internal_RegisterReadEvent(int fd, CLMessageReceiver *pMsgReceiver)
{
    map<int, CLMessageReceiver*>::iterator it = m_ReadSetMap.find(fd);
    if(it != m_ReadSetMap.end())
        return CLStatus(-1, NORMAL_ERROR);

    m_ReadSetMap[fd] = pMsgReceiver;

    struct event *ev;
    //实现On_Read函数
    ev = event_new(m_base, fd, EV_PERSIST | EV_READ, On_Read, fd);
    event_add(ev, NULL);

    return CLStatus(0, 0);
}
CLStatus CLMsgLoopManagerForLibevent::RegisterReadEvent(int fd, CLMessageReceiver *pMsgReceiver)
{
    if(pMsgReceiver == 0)
        return CLStatus(-1, NORMAL_ERROR);
    
    if(m_bMultipleThread)
    {
        CLCriticalSection cs(&m_MutexForReadMap);

        if(Internal_RegisterReadEvent(fd, pMsgReceiver).IsSuccess())
            return CLStatus(0, 0);
    }
    else
    {
        if(Internal_RegisterReadEvent(fd, pMsgReceiver).IsSuccess())
            return CLStatus(0, 0);
    }

    CLLogger::WriteLogMsg("In CLMsgLoopManagerForLibevent::RegisterReadEvent(), Internal_RegisterReadEvent error", -1);

    delete pMsgReceiver;

    return CLStatus(-1, NORMAL_ERROR);
}

CLStatus CLMsgLoopManagerForLibevent::UnRegisterReadEvent(int fd)
{
    if(m_bMultipleThread)
    {
        {
            CLCriticalSection cs(&m_MutexForDeletedSet);

            set<struct event*>::iterator it = m_DeletedSet.find(fd);
            if(it != m_DeletedSet.end())
                return CLStatus(0, 0);

            pair<set<int>::iterator, bool> r = m_DeletedSet.insert(fd);

            if(r.second)
                return CLStatus(0, 0);
        }

        CLLogger::WriteLogMsg("In CLMsgLoopManagerForLibevent::UnRegisterReadEvent(), m_DeletedSet.insert error", -1);

        return CLStatus(-1, NORMAL_ERROR);
    }
    else
    {
        CLStatus s = Internal_UnRegisterReadEvent(fd);
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

CLStatus CLMsgLoopManagerForLibevent::Internal_UnRegisterReadEvent(int fd)
{
    map<int, CLMessageReceiver*>::iterator it = m_ReadSetMap.find(fd);
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

		set<int>::iterator iter = m_DeletedSet.begin();
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

CLStatus CLMsgLoopManagerForLibevent::RegisterWriteEvent(int fd, CLMessagePoster *pMsgPoster)
{
    if(m_bMultipleThread)
    {
        CLCriticalSection cs(&m_MutexForWriteMap);

        return Internal_RegisterWriteEvent(fd, pMsgPoster);
    }
    else
    {
        return Internal_RegisterWriteEvent(fd, pMsgPoster);
    }
}

CLStatus CLMsgLoopManagerForLibevent::Internal_RegisterWriteEvent(int fd, CLMessagePoster *pMsgPoster)
{
    map<int, CLMessagePoster*>::iterator it = m_WriteSetMap.find(fd);
    if(it != m_WriteSetMap.end())
        return CLStatus(0, 0);

    m_WriteSetMap[fd] = pMsgPoster;

    struct event *ev;
    //实现On_Write函数
    ev = event_new(m_base, fd, EV_WRITE | EV_PERSIST, On_Write, fd);
    event_add(ev, NULL);

    return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForLibevent::Internal_RegisterConnectEvent(int fd, CLDataPostChannelMainter *pChannel)
{
    map<int, CLDataPostChannelMainter*>::iterator it = m_ChannelMap.find(fd);
    if(it != m_ChannelMap.end())
        return CLStatus(0, 0);

    m_ChannelMap[fd] = pChannel;

    Internal_RegisterReadEvent(fd, 0);

    Internal_RegisterWriteEvent(fd, 0);

    return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForLibevent::RegisterConnectEvent(int fd, CLDataPostChannelMainter *pChannel)
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

                return Internal_RegisterConnectEvent(fd, pChannel);
            }
        }
    }
    else
    {
        return Internal_RegisterConnectEvent(fd, pChannel);
    }
}

void CLMsgLoopManagerForLibevent::On_Read(int fd)
{

}

void CLMsgLoopManagerForLibevent::On_Write(int fd)
{

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
    event_base_dispatch(base);
}
