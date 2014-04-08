#ifndef CLMsgLoopManagerForIOMultiplexing_H
#define CLMsgLoopManagerForIOMultiplexing_H

#include <string>
#include <map>
#include <set>
#include <list>
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
	CLMsgLoopManagerForIOMultiplexing(CLMessageObserver *pMsgObserver, const char* pstrThreadName);
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
};

#endif