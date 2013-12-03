#ifndef CLEPOLLEVENT_H
#define CLEPOLLEVENT_H

#include "CLStatus.h"

class CLEpoll;

class CLEpollEvent
{
public:
	CLEpollEvent(CLEpoll* pEpoll);
	virtual ~CLEpollEvent();

	CLStatus SetHandler(void* pHandle);
	CLStatus SetFd(int fd);

	void* GetHandler();

	CLStatus RegisterREvent();
	CLStatus RegisterWEvent();
	CLStatus RegisterRWEvents();
	CLStatus UnRegisterRWEvents();

	CLStatus OpenREvent();
	CLStatus OpenWEvent();
	CLStatus CloseREvent();
	CLStatus CloseWEvent();

private:
	CLEpoll *m_pEpoll;
	void *m_pHandle;
	int m_Fd;
	unsigned int m_Event;

	bool m_bReadFlag;
	bool m_bWriteFlag;

private:
	CLEpollEvent(const CLEpollEvent&);
	CLEpollEvent& operator=(const CLEpollEvent&);
};

#endif