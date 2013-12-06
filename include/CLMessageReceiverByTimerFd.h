#ifndef CLMESSAGERECEIVERBYTIMERFD_H
#define CLMESSAGERECEIVERBYTIMERFD_H

#include <queue>
#include <string>

#include "CLStatus.h"
#include "CLMessageReceiver.h"

class CLMessage;
class CLTimerFd;

struct itimerspec;

class CLMessageReceiverByTimerFd : public CLMessageReceiver
{
public:
	CLMessageReceiverByTimerFd(CLTimerFd *pTimer);
	virtual ~CLMessageReceiverByTimerFd();

	virtual CLStatus GetMessage(std::queue<CLMessage*> &MessageQueue);
	virtual const int& GetFd();

private:
	CLMessageReceiverByTimerFd(const CLMessageReceiverByTimerFd&);
	CLMessageReceiverByTimerFd& operator=(const CLMessageReceiverByTimerFd&);

private:
	CLTimerFd *m_pTimer;
	//这里默认对方使用的接收信息的通道是跨进程的管道。当然后期应该要加入对方是服务器的网络端  的场景

};

#endif

