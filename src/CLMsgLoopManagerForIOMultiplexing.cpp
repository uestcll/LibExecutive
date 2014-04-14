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

CLMsgLoopManagerForIOMultiplexing::CLMsgLoopManagerForIOMultiplexing(CLMessageObserver *pMsgObserver, const char* pstrThreadName, bool bMultipleThread) : CLMessageLoopManager(pMsgObserver)
{
	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLMsgLoopManagerForIOMultiplexing::CLMsgLoopManagerForIOMultiplexing(), pstrThreadName error";

	m_strThreadName = pstrThreadName;

	m_pReadSet = new fd_set;
	m_pWriteSet = new fd_set;

	FD_ZERO(m_pReadSet);
	FD_ZERO(m_pWriteSet);

	m_bMultipleThread = bMultipleThread;
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

CLStatus CLMsgLoopManagerForIOMultiplexing::Internal_RegisterReadEvent(int fd, CLMessageReceiver *pMsgReceiver)
{
	map<int, CLMessageReceiver*>::iterator it = m_ReadSetMap.find(fd);
	if(it != m_ReadSetMap.end())
		return CLStatus(-1, NORMAL_ERROR);

	m_ReadSetMap[fd] = pMsgReceiver;

	FD_SET(fd, m_pReadSet);

	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForIOMultiplexing::RegisterReadEvent(int fd, CLMessageReceiver *pMsgReceiver)
{
	if(pMsgReceiver == 0)
		return CLStatus(-1, NORMAL_ERROR);

	if(fd < 0)
	{
		delete pMsgReceiver;
		return CLStatus(-1, NORMAL_ERROR);
	}

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

	CLLogger::WriteLogMsg("In CLMsgLoopManagerForIOMultiplexing::RegisterReadEvent(), Internal_RegisterReadEvent error", fd);

	delete pMsgReceiver;

	return CLStatus(-1, NORMAL_ERROR);
}

CLStatus CLMsgLoopManagerForIOMultiplexing::UnRegisterReadEvent(int fd)
{
	if(fd < 0)
		return CLStatus(-1, NORMAL_ERROR);

	if(m_bMultipleThread)
	{
		{
			CLCriticalSection cs(&m_MutexForDeletedSet);

			set<int>::iterator it = m_DeletedSet.find(fd);
			if(it != m_DeletedSet.end())
				return CLStatus(0, 0);

			pair<set<int>::iterator, bool> r = m_DeletedSet.insert(fd);
			if(r.second)
				return CLStatus(0, 0);
		}

		CLLogger::WriteLogMsg("In CLMsgLoopManagerForIOMultiplexing::UnRegisterReadEvent(), m_DeletedSet.insert error", fd);

		return CLStatus(-1, NORMAL_ERROR);
	}
	else
	{
		CLStatus s = Internal_UnRegisterReadEvent(fd);
		if(!s.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLMsgLoopManagerForIOMultiplexing::UnRegisterReadEvent(), Internal_UnRegisterReadEvent error", fd);
			return s;
		}

		CLMessageReceiver *pTmp = (CLMessageReceiver *)s.m_clErrorCode;

		if(pTmp)
			delete pTmp;

		return s;
	}
}

CLStatus CLMsgLoopManagerForIOMultiplexing::Internal_UnRegisterReadEvent(int fd)
{
	map<int, CLMessageReceiver*>::iterator it = m_ReadSetMap.find(fd);
	if(it == m_ReadSetMap.end())
		return CLStatus(-1, NORMAL_ERROR);

	CLMessageReceiver *pTmp = it->second;

	FD_CLR(it->first, m_pReadSet);

	m_ReadSetMap.erase(it);

	return CLStatus(0, (long)pTmp);
}

void CLMsgLoopManagerForIOMultiplexing::ClearDeletedSet()
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
				CLLogger::WriteLogMsg("In CLMsgLoopManagerForIOMultiplexing::ClearDeletedSet(), Internal_UnRegisterReadEvent error", *iter);
				continue;
			}

			CLMessageReceiver *pTmp = (CLMessageReceiver *)s1.m_clErrorCode;
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

CLStatus CLMsgLoopManagerForIOMultiplexing::RegisterWriteEvent(int fd, CLMessagePoster *pMsgPoster)
{
	if(fd < 0)
		return CLStatus(-1, NORMAL_ERROR);

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

CLStatus CLMsgLoopManagerForIOMultiplexing::Internal_RegisterWriteEvent(int fd, CLMessagePoster *pMsgPoster)
{
	map<int, CLMessagePoster*>::iterator it = m_WriteSetMap.find(fd);
	if(it != m_WriteSetMap.end())
		return CLStatus(0, 0);

	m_WriteSetMap[fd] = pMsgPoster;

	FD_SET(fd, m_pWriteSet);

	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForIOMultiplexing::Internal_RegisterConnectEvent(int fd, CLDataPostChannelMaintainer *pChannel)
{
	map<int, CLDataPostChannelMaintainer*>::iterator it = m_ChannelMap.find(fd);
	if(it != m_ChannelMap.end())
		return CLStatus(0, 0);

	m_ChannelMap[fd] = pChannel;

	Internal_RegisterReadEvent(fd, 0);

	Internal_RegisterWriteEvent(fd, 0);

	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForIOMultiplexing::RegisterConnectEvent(int fd, CLDataPostChannelMaintainer *pChannel)
{
	if((fd < 0) || (pChannel == 0))
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

CLStatus CLMsgLoopManagerForIOMultiplexing::Initialize()
{
	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForIOMultiplexing::Uninitialize()
{
	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForIOMultiplexing::GetInfoFromSet(bool bReadSet, fd_set *pSet, int& maxfd)
{
	if(bReadSet)
	{
		map<int, CLMessageReceiver*>::iterator it = m_ReadSetMap.rbegin();
		if(it != m_ReadSetMap.end())
		{
			maxfd = it->first;
			memcpy(pSet, m_pReadSet, sizeof(fd_set));
			return CLStatus(0, 0);
		}
		//................................
	}
}

CLStatus CLMsgLoopManagerForIOMultiplexing::GetSeletcParameters(fd_set *pReadSet, fd_set *pWriteSet, int& maxfdp1)
{
	int maxrfd = 0;
	int maxwfd = 0;

	if(m_bMultipleThread)
	{
		CLCriticalSection cs(&m_MutexForReadMap);

		maxrfd = m_ReadSetMap.rbegin()->first;
		memcpy(pReadSet, m_pReadSet, sizeof(fd_set));
	}
	else
	{
		maxrfd = m_ReadSetMap.rbegin()->first;
		memcpy(pReadSet, m_pReadSet, sizeof(fd_set));
	}

	if(m_bMultipleThread)
	{
		CLCriticalSection cs(&m_MutexForWriteMap);

		maxwfd = m_WriteSetMap.rbegin()->first;
		memcpy(pWriteSet, m_pWriteSet, sizeof(fd_set));
	}
	else
	{
		maxwfd = m_WriteSetMap.rbegin()->first;
		memcpy(pWriteSet, m_pWriteSet, sizeof(fd_set));
	}

	if(maxrfd > maxwfd)
		maxfdp1 = maxrfd + 1;
	else
		maxfdp1 = maxwfd + 1;

	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForIOMultiplexing::WaitForMessage()
{
	if(m_bMultipleThread)
		ClearDeletedSet();

	fd_set *pReadSet = new fd_set;
	fd_set *pWriteSet = new fd_set;
	FD_ZERO(pReadSet);
	FD_ZERO(pWriteSet);
	int maxfdp1 = -1;

	GetSeletcParameters(pReadSet, pWriteSet, maxfdp1);

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