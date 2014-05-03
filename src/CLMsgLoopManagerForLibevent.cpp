#include <string.h>
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
	map<struct event, CLMessageReceiver*>::iterator it = m_ReadSetMap.begin();
    for(; it != m_ReadSetMap.end(); ++it)
    {
        delete it->second;
    }
}

CLStatus CLMsgLoopManagerForLibevent::Internal_RegisterReadEvent(struct event ev, CLMessageReceiver *pMsgReceiver)
{
    map<struct event, CLMessageReceiver*>::iterator it = m_ReadSetMap.find(ev);
    if(it != m_ReadSetMap.end())
        return CLStatus(-1, NORMAL_ERROR);

    m_ReadSetMap[ev] = pMsgReceiver;

    return CLStatus(0, 0);
}
CLStatus CLMsgLoopManagerForLibevent::RegisterReadEvent(struct event ev, CLMessageReceiver *pMsgReceiver)
{
    if(pMsgReceiver == 0)
        return CLStatus(-1, NORMAL_ERROR);

}

CLStatus CLMsgLoopManagerForLibevent::UnRegisterReadEvent(struct event ev)
{
	CLCriticalSection cs(&m_MutexForDeletedSet);

	set<int>::iterator it = m_DeletedSet.find(ev);
	if(it != m_DeletedSet.end())
		return CLStatus(0, 0);

	pair<set<struct event>::iterator, bool> r = m_DeletedSet.insert(ev);
	if(r.second)
		return CLStatus(0, 0);
	
	CLLogger::WriteLogMsg("In CLMsgLoopManagerForIOMultiplexing::UnRegisterReadEvent(), m_DeletedSet.insert error", 0);
	
	return CLStatus(-1, NORMAL_ERROR);
}

CLStatus CLMsgLoopManagerForLibevent::RegisterWriteEvent(struct event ev, CLMessagePoster *pMsgPoster)
{
	return CLStatus(-1, NORMAL_ERROR);
}

void CLMsgLoopManagerForLibevent::ClearDeletedSet()
{
	vector<CLMessageReceiver *> vpMsgReceiver;
	
	{
		CLCriticalSection cs(&m_MutexForDeletedSet);

		set<int>::iterator iter = m_DeletedSet.begin();
		for(; iter != m_DeletedSet.end(); ++iter)
		{
			CLCriticalSection cs1(&m_MutexForReadMap);

			map<struct event, CLMessageReceiver*>::iterator it = m_ReadSetMap.find(*iter);
			if(it != m_ReadSetMap.end())
			{
				vpMsgReceiver.push_back(it->second);
				m_ReadSetMap.erase(it);
			}
			else
				CLLogger::WriteLogMsg("In CLMsgLoopManagerForIOMultiplexing::ClearDeletedSet(), m_ReadSetMap.find error", 0);
		}

		m_DeletedSet.clear();
	}

	vector<CLMessageReceiver*>::iterator it = vpMsgReceiver.begin();
	for(; it != vpMsgReceiver.end(); ++it)
	{
		delete (*it);
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

	return CLStatus(0, 0);
}
