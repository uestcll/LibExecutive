#include <string.h>
#include <utility>
#include <vector>
#include "CLMsgLoopManagerForIOMultiplexing.h"
#include "CLCriticalSection.h"
#include "ErrorCode.h"
#include "CLMessageReceiver.h"
#include "CLLogger.h"

using namespace std;

CLMsgLoopManagerForIOMultiplexing::CLMsgLoopManagerForIOMultiplexing(CLMessageObserver *pMsgObserver, const char* pstrThreadName) : CLMessageLoopManager(pMsgObserver)
{
	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLMsgLoopManagerForIOMultiplexing::CLMsgLoopManagerForIOMultiplexing(), pstrThreadName error";

	m_strThreadName = pstrThreadName;

	m_pReadSet = new fd_set;
	m_pWriteSet = new fd_set;
}

CLMsgLoopManagerForIOMultiplexing::~CLMsgLoopManagerForIOMultiplexing()
{
	ClearDeletedSet();

	delete m_pReadSet;

	delete m_pWriteSet;
}

CLStatus CLMsgLoopManagerForIOMultiplexing::RegisterReadEvent(int fd, CLMessageReceiver *pMsgReceiver)
{
	try
	{
		CLCriticalSection cs(&m_MutexForReadMap);

		map<int, CLMessageReceiver*>::iterator it = m_ReadSetMap.find(fd);
		if(it != m_ReadSetMap.end())
		{
			CLLogger::WriteLogMsg("In CLMsgLoopManagerForIOMultiplexing::RegisterReadEvent(), m_ReadSetMap.find error", 0);
			throw CLStatus(-1, NORMAL_ERROR);
		}

		m_ReadSetMap[fd] = pMsgReceiver;

		return CLStatus(0, 0);
	}
	catch(CLStatus& s)
	{
		if(pMsgReceiver)
			delete pMsgReceiver;

		return s;
	}
}

CLStatus CLMsgLoopManagerForIOMultiplexing::UnRegisterReadEvent(int fd)
{
	CLCriticalSection cs(&m_MutexForDeletedSet);

	set<int>::iterator it = m_DeletedSet.find(fd);
	if(it != m_DeletedSet.end())
		return CLStatus(0, 0);

	pair<set<int>::iterator, bool> r = m_DeletedSet.insert(fd);
	if(r.second)
		return CLStatus(0, 0);
	
	CLLogger::WriteLogMsg("In CLMsgLoopManagerForIOMultiplexing::UnRegisterReadEvent(), m_DeletedSet.insert error", 0);
	
	return CLStatus(-1, NORMAL_ERROR);
}

CLStatus CLMsgLoopManagerForIOMultiplexing::RegisterWriteEvent(int, CLProtocolDataPoster *pDataPoster)
{
	return CLStatus(-1, NORMAL_ERROR);
}

void CLMsgLoopManagerForIOMultiplexing::ClearDeletedSet()
{
	vector<CLMessageReceiver *> vpMsgReceiver;
	
	{
		CLCriticalSection cs(&m_MutexForDeletedSet);

		set<int>::iterator iter = m_DeletedSet.begin();
		for(; iter != m_DeletedSet.end(); ++iter)
		{
			CLCriticalSection cs1(&m_MutexForReadMap);

			map<int, CLMessageReceiver*>::iterator it = m_ReadSetMap.find(*iter);
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

CLStatus CLMsgLoopManagerForIOMultiplexing::Initialize()
{
	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForIOMultiplexing::Uninitialize()
{
	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForIOMultiplexing::WaitForMessage()
{
	ClearDeletedSet();

	FD_ZERO(m_pReadSet);
	FD_ZERO(m_pWriteSet);

	int maxrfd = 0;
	int maxwfd = 0;

	{
		CLCriticalSection cs(&m_MutexForReadMap);

		maxrfd = m_ReadSetMap.rbegin()->first;

		map<int, CLMessageReceiver*>::iterator it = m_ReadSetMap.begin();
		for(; it != m_ReadSetMap.end(); ++it)
		{
			FD_SET(it->first, m_pReadSet);
		}
	}

	{
		CLCriticalSection cs(&m_MutexForWriteMap);

		maxwfd = m_WriteSetMap.rbegin()->first;
		
		map<int, std::list<CLProtocolDataPoster*>*>::iterator it = m_WriteSetMap.begin();
		for(; it != m_WriteSetMap.end(); ++it)
		{
			FD_SET(it->first, m_pWriteSet);
		}
	}

	int maxfdp1 = 0;
	if(maxrfd > maxwfd)
		maxfdp1 = maxrfd + 1;
	else
		maxfdp1 = maxwfd + 1;

	int ready_fd = select(maxfdp1, m_pReadSet, m_pWriteSet, 0, 0);

	vector<CLMessageReceiver *> vpMsgReceiver;
	
	{
		CLCriticalSection cs(&m_MutexForReadMap);

		map<int, CLMessageReceiver*>::iterator it = m_ReadSetMap.begin();
		for(; it != m_ReadSetMap.end(); ++it)
		{
			if(FD_ISSET(it->first, m_pReadSet))
			{
				vpMsgReceiver.push_back(it->second);
			}
		}
	}

	//.....................

	return CLStatus(0, 0);
}