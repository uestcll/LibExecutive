#include <sys/socket.h>
#include <string.h>
#include <utility>
#include <vector>
#include "CLMsgLoopManagerForIOMultiplexing.h"
#include "CLCriticalSection.h"
#include "ErrorCode.h"
#include "CLMessageReceiver.h"
#include "CLMessagePoster.h"
#include "CLLogger.h"
#include "CLDataPostChannelMaintainer.h"

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
		//delete it->second;
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

	// if(-1 == close(fd))
	// {
	// 	CLLogger::WriteLogMsg("In CLMsgLoopManagerForIOMultiplexing::UnRegisterReadEvent(), close fd error", fd);
	// }

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
	if((fd < 0) || (pMsgPoster == 0))
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

CLStatus CLMsgLoopManagerForIOMultiplexing::UnRegisterWriteEvent(int fd)
{
	if(fd < 0)
		return CLStatus(-1, NORMAL_ERROR);

	if(m_bMultipleThread)
	{
		CLCriticalSection cs(&m_MutexForWriteMap);

		return Internal_UnRegisterWriteEvent(fd);
	}
	else
	{
		return Internal_UnRegisterWriteEvent(fd);
	}
}

CLStatus CLMsgLoopManagerForIOMultiplexing::Internal_UnRegisterWriteEvent(int fd)
{
	map<int, CLMessagePoster*>::iterator it = m_WriteSetMap.find(fd);
	if(it == m_WriteSetMap.end())
		return CLStatus(-1, NORMAL_ERROR);

	FD_CLR(fd, m_pWriteSet);

	m_WriteSetMap.erase(it);

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

CLStatus CLMsgLoopManagerForIOMultiplexing::GetInfoFromSet(bool bReadSet, fd_set **ppSet, int& maxfd)
{
	if(bReadSet)
	{
		map<int, CLMessageReceiver*>::reverse_iterator it = m_ReadSetMap.rbegin();
		if(it != m_ReadSetMap.rend())
		{
			maxfd = it->first;

			*ppSet = new fd_set;
			FD_ZERO(*ppSet);

			memcpy(*ppSet, m_pReadSet, sizeof(fd_set));
			return CLStatus(0, 0);
		}
	}
	else
	{
		map<int, CLMessagePoster*>::reverse_iterator it = m_WriteSetMap.rbegin();
		if(it != m_WriteSetMap.rend())
		{
			maxfd = it->first;

			*ppSet = new fd_set;
			FD_ZERO(*ppSet);

			memcpy(*ppSet, m_pWriteSet, sizeof(fd_set));
			return CLStatus(0, 0);
		}
	}

	*ppSet = 0;
	return CLStatus(-1, NORMAL_ERROR);
}

void CLMsgLoopManagerForIOMultiplexing::GetSelectParameters(fd_set **ppReadSet, fd_set **ppWriteSet, int& maxfdp1)
{
	int maxrfd = -1;
	int maxwfd = -1;

	if(m_bMultipleThread)
	{
		CLCriticalSection cs(&m_MutexForReadMap);

		GetInfoFromSet(true, ppReadSet, maxrfd);
	}
	else
	{
		GetInfoFromSet(true, ppReadSet, maxrfd);
	}

	if(m_bMultipleThread)
	{
		CLCriticalSection cs(&m_MutexForWriteMap);

		GetInfoFromSet(false, ppWriteSet, maxwfd);
	}
	else
	{
		GetInfoFromSet(false, ppWriteSet, maxwfd);
	}

	if((maxrfd == -1) && (maxwfd == -1))
	{
		maxfdp1 = -1;
		return;
	}
		
	if(maxrfd > maxwfd)
		maxfdp1 = maxrfd + 1;
	else
		maxfdp1 = maxwfd + 1;

	return;
}

void CLMsgLoopManagerForIOMultiplexing::Internal_ProcessConnectEvent(vector<pair<int, CLDataPostChannelMaintainer *> >& vSuccessChannel, vector<pair<int, CLDataPostChannelMaintainer *> >& vFailureChannel, fd_set *pReadSet, fd_set *pWriteSet)
{
	map<int, CLDataPostChannelMaintainer*>::iterator it = m_ChannelMap.begin();
	for(; it != m_ChannelMap.end(); ++it)
	{
		int fd = it->first;

		if(FD_ISSET(fd, pWriteSet))
		{
			if((pReadSet != 0) && (FD_ISSET(fd, pReadSet)))
			{
				long err = 0;
				socklen_t len = sizeof(long);
				if((getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0) || (err != 0))
				{
					pair<int, CLDataPostChannelMaintainer*> p(fd, it->second);
					vFailureChannel.push_back(p);

					continue;
				}
			}

			pair<int, CLDataPostChannelMaintainer*> p(fd, it->second);
			vSuccessChannel.push_back(p);

			continue;
		}
	}
}

void CLMsgLoopManagerForIOMultiplexing::Internal_ProcessConnectWriteEvent(vector<pair<int, CLDataPostChannelMaintainer *> >& vSuccessChannel, vector<pair<int, CLDataPostChannelMaintainer *> >& vFailureChannel)
{
	vector<pair<int, CLDataPostChannelMaintainer *> >::iterator it = vSuccessChannel.begin();
	for(; it != vSuccessChannel.end(); ++it)
	{
		Internal_UnRegisterWriteEvent(it->first);
	}

	vector<pair<int, CLDataPostChannelMaintainer *> >::iterator it1 = vFailureChannel.begin();
	for(; it1 != vFailureChannel.end(); ++it1)
	{
		Internal_UnRegisterWriteEvent(it1->first);
	}
}

void CLMsgLoopManagerForIOMultiplexing::Internal_ProcessConnectReadEvent(vector<pair<int, CLDataPostChannelMaintainer *> >& vSuccessChannel, vector<pair<int, CLDataPostChannelMaintainer *> >& vFailureChannel)
{
	vector<pair<int, CLDataPostChannelMaintainer *> >::iterator it = vSuccessChannel.begin();
	for(; it != vSuccessChannel.end(); ++it)
	{
		Internal_UnRegisterReadEvent(it->first);
	}

	vector<pair<int, CLDataPostChannelMaintainer *> >::iterator it1 = vFailureChannel.begin();
	for(; it1 != vFailureChannel.end(); ++it1)
	{
		Internal_UnRegisterReadEvent(it1->first);
	}
}

void CLMsgLoopManagerForIOMultiplexing::Internal_ProcessConnectChannelEvent(vector<pair<int, CLDataPostChannelMaintainer *> >& vSuccessChannel, vector<pair<int, CLDataPostChannelMaintainer *> >& vFailureChannel, fd_set *pReadSet, fd_set *pWriteSet)
{
	Internal_ProcessConnectEvent(vSuccessChannel, vFailureChannel, pReadSet, pWriteSet);

	vector<pair<int, CLDataPostChannelMaintainer *> >::iterator it = vSuccessChannel.begin();
	for(; it != vSuccessChannel.end(); ++it)
	{
		m_ChannelMap.erase(it->first);
	}

	vector<pair<int, CLDataPostChannelMaintainer *> >::iterator it1 = vFailureChannel.begin();
	for(; it1 != vFailureChannel.end(); ++it1)
	{
		m_ChannelMap.erase(it1->first);
	}
}

void CLMsgLoopManagerForIOMultiplexing::ProcessConnectEvent(fd_set *pReadSet, fd_set *pWriteSet)
{
	if(pWriteSet == 0)
		return;

	vector<pair<int, CLDataPostChannelMaintainer *> > vSuccessChannel;
	vector<pair<int, CLDataPostChannelMaintainer *> > vFailureChannel;

	if(m_bMultipleThread)
	{
		CLCriticalSection cs(&m_MutexForChannelMap);

		Internal_ProcessConnectChannelEvent(vSuccessChannel, vFailureChannel, pReadSet, pWriteSet);

	}
	else
	{
		Internal_ProcessConnectChannelEvent(vSuccessChannel, vFailureChannel, pReadSet, pWriteSet);
	}

	if(m_bMultipleThread)
	{
		CLCriticalSection cs(&m_MutexForReadMap);

		Internal_ProcessConnectReadEvent(vSuccessChannel, vFailureChannel);
	}
	else
	{
		Internal_ProcessConnectReadEvent(vSuccessChannel, vFailureChannel);
	}

	if(m_bMultipleThread)
	{
		CLCriticalSection cs(&m_MutexForWriteMap);

		Internal_ProcessConnectWriteEvent(vSuccessChannel, vFailureChannel);
	}
	else
	{
		Internal_ProcessConnectWriteEvent(vSuccessChannel, vFailureChannel);
	}

	vector<pair<int, CLDataPostChannelMaintainer *> >::iterator it = vSuccessChannel.begin();
	for(; it != vSuccessChannel.end(); ++it)
	{
		it->second->NotifyConnectionResult(true);
	}

	vector<pair<int, CLDataPostChannelMaintainer *> >::iterator it1 = vFailureChannel.begin();
	for(; it1 != vFailureChannel.end(); ++it1)
	{
		it1->second->NotifyConnectionResult(false);
	}

	return;
}

void CLMsgLoopManagerForIOMultiplexing::ProcessWriteEvent(fd_set *pWriteSet)
{
	if(pWriteSet == 0)
		return;

	vector<pair<int, CLMessagePoster *> >vpMsgPoster;

	if(m_bMultipleThread)
	{
		CLCriticalSection cs(&m_MutexForWriteMap);
		
		Internal_ProcessWriteEvent(vpMsgPoster, pWriteSet);
	}
	else
	{
		Internal_ProcessWriteEvent(vpMsgPoster, pWriteSet);
	}

	vector<pair<int, CLMessagePoster *> >::iterator it = vpMsgPoster.begin();
	
	for(; it != vpMsgPoster.end(); ++it)
	{
		CLStatus ss = UnRegisterWriteEvent(it->first);
		if(!ss.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLMsgLoopManagerForIOMultiplexing::ProcessWriteEvent(), UnRegisterWriteEvent() error", it->first);
		}

		CLStatus s = it->second->NotifyMsgPosterReadyToPost();
		if(!s.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLMsgLoopManagerForIOMultiplexing::ProcessWriteEvent(), it->second->NotifyPoster() error", 0);
		}
	}

	return;
}

void CLMsgLoopManagerForIOMultiplexing::Internal_ProcessWriteEvent(vector<pair<int, CLMessagePoster *> >& vMsgPoster, fd_set *pWriteSet)
{
	map<int, CLMessagePoster*>::iterator it = m_WriteSetMap.begin();
	for(; it != m_WriteSetMap.end(); ++it)
	{
		if(FD_ISSET(it->first, pWriteSet))
		{
			pair<int, CLMessagePoster*> p(it->first, it->second);
			vMsgPoster.push_back(p);
		}
	}

	return;
}

void CLMsgLoopManagerForIOMultiplexing::ProcessReadEvent(fd_set *pReadSet)
{
	if(pReadSet == 0)
		return;

	vector<pair<int, CLMessageReceiver *> > vpMsgReceiver;
	
	if(m_bMultipleThread)
	{
		CLCriticalSection cs(&m_MutexForReadMap);

		Internal_ProcessReadEvent(vpMsgReceiver, pReadSet);
	}
	else
	{
		Internal_ProcessReadEvent(vpMsgReceiver, pReadSet);
	}

	vector<pair<int, CLMessageReceiver*> >::iterator iter_read = vpMsgReceiver.begin();
	for(; iter_read != vpMsgReceiver.end(); ++iter_read)
	{
		CLStatus s = iter_read->second->GetMessage(m_MessageContainer);
		if(!s.IsSuccess() && (s.m_clErrorCode == NORMAL_ERROR))
		{
			ProcessErrorReadEvent(iter_read->first);
		}
	}
}

void CLMsgLoopManagerForIOMultiplexing::Internal_ProcessReadEvent(vector<pair<int, CLMessageReceiver*> >& vMsgReceiver, fd_set *pReadSet)
{
	map<int, CLMessageReceiver* >::iterator it = m_ReadSetMap.begin();
	for(; it != m_ReadSetMap.end(); ++ it)
	{
		if(FD_ISSET(it->first, pReadSet))
		{
			pair<int, CLMessageReceiver*> p(it->first, it->second);
			vMsgReceiver.push_back(p);
		}
	}

	return;
}

void CLMsgLoopManagerForIOMultiplexing::ProcessErrorReadEvent(int fd)
{
	if(m_bMultipleThread)
	{
		CLCriticalSection cs(&m_MutexForReadMap);

		FD_CLR(fd, m_pReadSet);
	}
	else
	{
		FD_CLR(fd, m_pReadSet);
	}

	return;
}

CLStatus CLMsgLoopManagerForIOMultiplexing::WaitForMessage()
{
	if(m_bMultipleThread)
		ClearDeletedSet();

	fd_set *pReadSet = 0;
	fd_set *pWriteSet = 0;
	int maxfdp1 = -1;
	
	try
	{
		GetSelectParameters(&pReadSet, &pWriteSet, maxfdp1);

		if(maxfdp1 == -1)
			throw CLStatus(0, 0);

		int ready_fd = select(maxfdp1, pReadSet, pWriteSet, 0, 0);

		if(ready_fd <= 0)
		{
			throw CLStatus(0, 0);
		}

		ProcessConnectEvent(pReadSet, pWriteSet);

		ProcessWriteEvent(pWriteSet);

		ProcessReadEvent(pReadSet);

		throw CLStatus(0, 0);
	}
	catch(CLStatus& s)
	{
		if(pReadSet)
			delete pReadSet;

		if(pWriteSet)
			delete pWriteSet;
	}

	return CLStatus(0, 0);	
}