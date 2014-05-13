#ifndef CLMsgLoopManagerForIOMultiplexing_H
#define CLMsgLoopManagerForIOMultiplexing_H

#include <string>
#include <map>
#include <set>
#include <utility>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "CLMessageLoopManager.h"
#include "CLMutex.h"

class CLMessageReceiver;
class CLMessagePoster;
class CLDataPostChannelMaintainer;

class CLMsgLoopManagerForIOMultiplexing : public CLMessageLoopManager
{
public:
	CLMsgLoopManagerForIOMultiplexing(CLMessageObserver *pMsgObserver, const char* pstrThreadName, bool bMultipleThread);
	virtual ~CLMsgLoopManagerForIOMultiplexing();

	CLStatus RegisterReadEvent(int fd, CLMessageReceiver *pMsgReceiver);
	CLStatus UnRegisterReadEvent(int fd);

	CLStatus RegisterWriteEvent(int fd, CLMessagePoster *pMsgPoster);

	CLStatus RegisterConnectEvent(int fd, CLDataPostChannelMaintainer *pChannel);

protected:
	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();

	virtual CLStatus WaitForMessage();

private:
	void ClearDeletedSet();

	CLStatus Internal_RegisterReadEvent(int fd, CLMessageReceiver *pMsgReceiver);
	CLStatus Internal_UnRegisterReadEvent(int fd);

	CLStatus Internal_RegisterWriteEvent(int fd, CLMessagePoster *pMsgPoster);
	CLStatus Internal_UnRegisterWriteEvent(int fd);

	CLStatus Internal_RegisterConnectEvent(int fd, CLDataPostChannelMaintainer *pChannel);

	CLStatus GetSelectParameters(fd_set **ppReadSet, fd_set **ppWriteSet, int& maxfdp1);

	CLStatus GetInfoFromSet(bool bReadSet, fd_set **ppSet, int& maxfd);

	CLStatus ProcessConnectEvent(fd_set *pReadSet, fd_set *pWriteSet);

	//using boost tuple........................adding 
	CLStatus Internal_ProcessConnectEvent(vector<pair<int, CLDataPostChannelMaintainer *> >& vSuccessChannel);

private:
	CLMsgLoopManagerForIOMultiplexing(const CLMsgLoopManagerForIOMultiplexing&);
	CLMsgLoopManagerForIOMultiplexing& operator=(const CLMsgLoopManagerForIOMultiplexing&);

private:
	std::string m_strThreadName;

	fd_set *m_pReadSet;
	fd_set *m_pWriteSet;

	std::map<int, CLMessageReceiver*> m_ReadSetMap;
	std::map<int, CLMessagePoster*> m_WriteSetMap;
	std::map<int, CLDataPostChannelMaintainer*> m_ChannelMap;
	std::set<int> m_DeletedSet;

	CLMutex m_MutexForReadMap;
	CLMutex m_MutexForWriteMap;
	CLMutex m_MutexForDeletedSet;
	CLMutex m_MutexForChannelMap;

	bool m_bMultipleThread;
};

#endif