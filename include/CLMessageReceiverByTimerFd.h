#ifndef CLMESSAGERECEIVERBYTIMERFD_H
#define CLMESSAGERECEIVERBYTIMERFD_H

#include <queue>
#include <string>

#include "CLStatus.h"

class CLMessage;
class CLTimerApplyMsg;


class CLMessageReceiverByTimerFd
{
public:
	CLMessageReceiverByTimerFd(CLTimerApplyMsg *pMsg);
	virtual ~CLMessageReceiverByTimerFd();

	virtual CLStatus GetMessage(std::queue<CLMessage*> &MessageQueue);
	virtual const int& GetFd();

private:
	CLMessageReceiverByTimerFd(const CLMessageReceiverByTimerFd&);
	CLMessageReceiverByTimerFd& operator=(const CLMessageReceiverByTimerFd&);

private:
	int m_Fd;
	std::string m_strRemoteName;
	//这里默认对方使用的接收信息的通道是跨进程的管道。当然后期应该要加入对方是服务器的网络端  的场景
	int m_iEchoID;
	struct itimerspec m_sTimerValue;
	bool m_bRepeat;
};

#endif

