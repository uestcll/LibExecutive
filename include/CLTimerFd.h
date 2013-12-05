#ifndef CLTIMERFD_H
#define CLTIMERFD_H

#include <string>
#include "CLStatus.h"

using namespace std;

class CLTimerFd
{
public:
	CLTimerFd(struct itimerspec& sTimerValue, string pstrRemoteName, const int& ID);
	virtual ~CLTimerFd();

public:
	CLStatus startClock();
	CLStatus Read();

	const bool& IsRepeatTimer();
	const string& GetRemoteName();
	const int& GetTimerFd();
	const int& GetTimerID();

private:
	string m_strRemoteName;
	struct itimerspec m_sTimerValue;
	int m_ID;
	bool m_bIsRepeat;
	int m_Fd;

private:
	CLTimerFd(const CLTimerFd&);
	CLTimerFd& operator=(const CLTimerFd&);

};