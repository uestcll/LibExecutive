#ifndef CLMsgLoopManagerForIOMultiplexing_H
#define CLMsgLoopManagerForIOMultiplexing_H

#include <string>
#include <map>
#include <set>
#include <vector>
#include <utility>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "CLMessageLoopManager.h"
#include "CLMutex.h"

class CLMessageReceiver;
class CLMessagePoster;
class CLDataPostChannelMaintainer;
class CLMsgReceiverReleaser;

struct SLMsgReceiverAndReleaser
{
	CLMessageReceiver *m_pReceiver;
	CLMsgReceiverReleaser *m_pReleaser;
};

class CLMsgLoopManagerForIOMultiplexing : public CLMessageLoopManager
{
public:
	CLMsgLoopManagerForIOMultiplexing(CLMessageObserver *pMsgObserver, const char* pstrThreadName, bool bMultipleThread);
	virtual ~CLMsgLoopManagerForIOMultiplexing();

	CLStatus RegisterReadEvent(int fd, CLMessageReceiver *pMsgReceiver, CLMsgReceiverReleaser *pReceiverReleaser = NULL);
	CLStatus UnRegisterReadEvent(int fd);

	CLStatus RegisterWriteEvent(int fd, CLMessagePoster *pMsgPoster);

	CLStatus RegisterConnectEvent(int fd, CLDataPostChannelMaintainer *pChannel);

protected:
	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();

	virtual CLStatus WaitForMessage();

private:
	void ClearDeletedSet();

	CLStatus UnRegisterWriteEvent(int fd);

	CLStatus Internal_RegisterReadEvent(int fd, SLMsgReceiverAndReleaser *pMsgReceiverAndReleaser);
	CLStatus Internal_UnRegisterReadEvent(int fd);

	CLStatus Internal_RegisterWriteEvent(int fd, CLMessagePoster *pMsgPoster);
	CLStatus Internal_UnRegisterWriteEvent(int fd);

	CLStatus Internal_RegisterConnectEvent(int fd, CLDataPostChannelMaintainer *pChannel);

	void GetSelectParameters(fd_set **ppReadSet, fd_set **ppWriteSet, int& maxfdp1);

	CLStatus GetInfoFromSet(bool bReadSet, fd_set **ppSet, int& maxfd);

	void ProcessConnectEvent(fd_set *pReadSet, fd_set *pWriteSet);

	void Internal_ProcessConnectEvent(std::vector<std::pair<int, CLDataPostChannelMaintainer *> >& vSuccessChannel, std::vector<std::pair<int, CLDataPostChannelMaintainer *> >& vFailureChannel, fd_set *pReadSet, fd_set *pWriteSet);
	void Internal_ProcessConnectChannelEvent(std::vector<std::pair<int, CLDataPostChannelMaintainer *> >& vSuccessChannel, std::vector<std::pair<int, CLDataPostChannelMaintainer *> >& vFailureChannel, fd_set *pReadSet, fd_set *pWriteSet);
	void Internal_ProcessConnectReadEvent(std::vector<std::pair<int, CLDataPostChannelMaintainer *> >& vSuccessChannel, std::vector<std::pair<int, CLDataPostChannelMaintainer *> >& vFailureChannel);
	void Internal_ProcessConnectWriteEvent(std::vector<std::pair<int, CLDataPostChannelMaintainer *> >& vSuccessChannel, std::vector<std::pair<int, CLDataPostChannelMaintainer *> >& vFailureChannel);

	void ProcessWriteEvent(fd_set *pWriteSet);
	void ProcessReadEvent(fd_set *pReadSet);

	void Internal_ProcessWriteEvent(std::vector<std::pair<int, CLMessagePoster *> >& vMsgPoster, fd_set *pWriteSet);
	void Internal_ProcessReadEvent(std::vector<std::pair<int, CLMessageReceiver*> >& vMsgReceiver, fd_set *pReadSet);

	void ProcessErrorReadEvent(int fd);

	void ReleaseMessageReceiver(SLMsgReceiverAndReleaser *pInfo);


private:
	CLMsgLoopManagerForIOMultiplexing(const CLMsgLoopManagerForIOMultiplexing&);
	CLMsgLoopManagerForIOMultiplexing& operator=(const CLMsgLoopManagerForIOMultiplexing&);

private:
	std::string m_strThreadName;

	fd_set *m_pReadSet;
	fd_set *m_pWriteSet;

	std::map<int, SLMsgReceiverAndReleaser*> m_ReadSetMap;
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