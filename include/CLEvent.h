#ifndef CLEVENT_H
#define CLEVENT_H

#include <string>
#include "CLStatus.h"
#include "CLMutex.h"
#include "CLConditionVariable.h"

using namespace std;

struct SLEventInfo;

class CLEvent
{
public: 
	CLEvent( );
	explicit CLEvent(bool bSemaphore);
	explicit CLEvent(const char *pstrEventName);
	CLEvent(const char *pstrEventName, bool bSemaphore);
	virtual ~CLEvent();

public: 
	CLStatus Set();
	CLStatus Wait();

private:
	CLEvent(const CLEvent&);
	CLEvent& operator=(const CLEvent&);

private:
	CLMutex m_Mutex;
	CLConditionVariable m_Cond;
	SLEventInfo *m_pEventInfo;
	bool m_bNeededDestroy;
	string m_strEventName;
};

#endif