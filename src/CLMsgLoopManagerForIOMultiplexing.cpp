#include <string.h>
#include <utility>
#include <vector>
#include "CLMsgLoopManagerForIOMultiplexing.h"
#include "CLCriticalSection.h"
#include "ErrorCode.h"
#include "CLMessageReceiver.h"
#include "CLMessagePoster.h"
#include "CLLogger.h"

using namespace std;

CLMsgLoopManagerForIOMultiplexing::CLMsgLoopManagerForIOMultiplexing(CLMessageObserver *pMsgObserver, const char* pstrThreadName) : CLMessageLoopManager(pMsgObserver)
{
	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLMsgLoopManagerForIOMultiplexing::CLMsgLoopManagerForIOMultiplexing(), pstrThreadName error";

	m_strThreadName = pstrThreadName;

	m_pReadSet = new fd_set;
	m_pWriteSet = new fd_set;

	FD_ZERO(m_pReadSet);
	FD_ZERO(m_pWriteSet);
}

CLMsgLoopManagerForIOMultiplexing::~CLMsgLoopManagerForIOMultiplexing()
{
	map<int, CLMessageReceiver*>::iterator it = m_ReadSetMap.begin();
	for(; it != m_ReadSetMap.end(); ++it)
	{
		delete it->second;
	}

	delete m_pReadSet;

	delete m_pWriteSet;
}

CLStatus CLMsgLoopManagerForIOMultiplexing::RegisterReadEvent(int fd, CLMessageReceiver *pMsgReceiver)
{
	try
	{
		if(fd < 0)
			throw CLStatus(-1, NORMAL_ERROR);

		CLCriticalSection cs(&m_MutexForReadMap);

		map<int, CLMessageReceiver*>::iterator it = m_ReadSetMap.find(fd);
		if(it != m_ReadSetMap.end())
		{
			CLLogger::WriteLogMsg("In CLMsgLoopManagerForIOMultiplexing::RegisterReadEvent(), m_ReadSetMap.find error", 0);
			throw CLStatus(-1, NORMAL_ERROR);
		}

		m_ReadSetMap[fd] = pMsgReceiver;

		FD_SET(fd, m_pReadSet);

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
	if(fd < 0)
		return CLStatus(-1, NORMAL_ERROR);

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

				FD_CLR(it->first, m_pReadSet);

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

CLStatus CLMsgLoopManagerForIOMultiplexing::RegisterWriteEvent(int fd, CLMessagePoster *pMsgPoster)
{
	if(fd < 0)
		return CLStatus(-1, NORMAL_ERROR);

	CLCriticalSection cs(&m_MutexForWriteMap);

	map<int, CLMessagePoster*>::iterator it = m_WriteSetMap.find(fd);
	if(it != m_WriteSetMap.end())
		return CLStatus(0, 0);

	m_WriteSetMap[fd] = pMsgPoster;

	FD_SET(fd, m_pWriteSet);

	return CLStatus(0, 0);
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

	fd_set *pReadSet = new fd_set;
	fd_set *pWriteSet = new fd_set;

	int maxrfd = 0;
	int maxwfd = 0;

	{
		CLCriticalSection cs(&m_MutexForReadMap);

		maxrfd = m_ReadSetMap.rbegin()->first;

		memcpy(pReadSet, m_pReadSet, sizeof(fd_set));
	}

	{
		CLCriticalSection cs(&m_MutexForWriteMap);

		maxwfd = m_WriteSetMap.rbegin()->first;
		
		memcpy(pWriteSet, m_pWriteSet, sizeof(fd_set));
	}

	int maxfdp1 = 0;
	if(maxrfd > maxwfd)
		maxfdp1 = maxrfd + 1;
	else
		maxfdp1 = maxwfd + 1;

	int ready_fd = select(maxfdp1, pReadSet, pWriteSet, 0, 0);

	vector<CLMessageReceiver *> vpMsgReceiver;
	
	{
		CLCriticalSection cs(&m_MutexForReadMap);

		map<int, CLMessageReceiver*>::iterator it = m_ReadSetMap.begin();
		for(; it != m_ReadSetMap.end(); ++it)
		{
			if(FD_ISSET(it->first, pReadSet))
			{
				vpMsgReceiver.push_back(it->second);
			}
		}
	}

	vector<CLMessageReceiver *>::iterator iter_read = vpMsgReceiver.begin();
	for(; iter_read != vpMsgReceiver.end(); ++iter_read)
	{
		//bug iff getmessage return error how to notify user
		(*iter_read)->GetMessage(m_MessageContainer);
	}

	{
		CLCriticalSection cs(&m_MutexForWriteMap);

		map<int, list<CLProtocolDataPoster*>*>::iterator it = m_WriteSetMap.begin();
		for(; it != m_WriteSetMap.end(); ++it)
		{
			if(FD_ISSET(it->first, pWriteSet))
			{
				//.......................
			}
		}
	}

	delete pReadSet;

	delete pWriteSet;

	return CLStatus(0, 0);
}