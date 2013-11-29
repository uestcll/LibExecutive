#ifndef CLDATARECEIVERBYTIMERFD_H
#define CLDATARECEIVERBYTIMERFD_H

#include "CLDataReceiver.h"

#include <string>

class CLDataReceiverByTimerFd : CLDataReceiver
{
public:
	CLDataReceiverByTimerFd(int fd, const char* pstrWaitingObjName); //等待超时的对象的名字（通过该名字，向其发送超时消息，该名字可能代表的是pipe也可能是网络）
	virtual CLDataReceiverByTimerFd();

	virtual CLStatus GetData(CLBuffer *pBuffer);
	virtual const int GetFd();

private:
	int m_Fd;
	std::string m_strRemoteName;
	int m_iEchoID;

private:
	CLDataReceiverByTimerFd(const CLDataReceiverByTimerFd& );
	CLDataReceiverByTimerFd& operator=(const CLDataReceiverByTimerFd&);
}; 