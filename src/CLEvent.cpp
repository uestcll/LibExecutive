#include "CLEvent.h"
#include "CLCriticalSection.h"
#include "CLLogger.h"
#include "CLSharedEventImpl.h"
#include "CLSharedEventAllocator.h"

CLEvent::CLEvent()
{
	m_pEventInfo = new SLEventInfo;

	m_pEventInfo->bSemaphore = 0;
	m_pEventInfo->Flag = 0;
	m_pEventInfo->Context = 0;

	m_bNeededDestroy = true;

	m_strEventName = "";
}

CLEvent::CLEvent(bool bSemaphore)
{
	m_pEventInfo = new SLEventInfo;

	if(bSemaphore)
		m_pEventInfo->bSemaphore = 1;
	else
		m_pEventInfo->bSemaphore = 0;

	m_pEventInfo->Flag = 0;
	m_pEventInfo->Context = 0;

	m_bNeededDestroy = true;

	m_strEventName = "";
}

CLEvent::CLEvent(const char *pstrEventName) : m_Mutex(pstrEventName, MUTEX_USE_SHARED_PTHREAD), m_Cond(pstrEventName)
{
	m_pEventInfo = CLSharedEventAllocator::Get(pstrEventName);
	if(m_pEventInfo == 0)
	{
		CLLogger::WriteLogMsg("In CLEvent::CLEvent1(), CLSharedEventAllocator::Get error", 0);
		throw "In CLEvent::CLEvent(), CLSharedEventAllocator::Get error";
	}

	m_strEventName = pstrEventName;
	m_bNeededDestroy = false;
}

CLEvent::CLEvent(const char *pstrEventName, bool bSemaphore) : m_Mutex(pstrEventName, MUTEX_USE_SHARED_PTHREAD), m_Cond(pstrEventName)
{
	m_pEventInfo = CLSharedEventAllocator::Get(pstrEventName);
	if(m_pEventInfo == 0)
	{
		CLLogger::WriteLogMsg("In CLEvent::CLEvent(), CLSharedEventAllocator::Get error", 0);
		throw "In CLEvent::CLEvent(), CLSharedEventAllocator::Get error";
	}

	m_strEventName = pstrEventName;

	if(bSemaphore)
		m_pEventInfo->bSemaphore = 1;
	else
		m_pEventInfo->bSemaphore = 0;

	m_bNeededDestroy = false;
}

CLEvent::~CLEvent()
{
	if(m_bNeededDestroy)
		delete m_pEventInfo;
	else
	{
		CLStatus s = CLSharedEventAllocator::Release(m_strEventName.c_str());
		if(!s.IsSuccess())
			CLLogger::WriteLogMsg("In CLEvent::~CLEvent(), CLSharedEventAllocator::Release error", 0);
	}
}

CLStatus CLEvent::Set()
{
	try
	{
		CLCriticalSection cs(&m_Mutex);

		m_pEventInfo->Flag = m_pEventInfo->Flag + 1;
	}
	catch(const char *str)
	{
		CLLogger::WriteLogMsg("In CLEvent::Set(), exception arise", 0);
		return CLStatus(-1, 0);
	}

	CLStatus s = m_Cond.Wakeup();
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLEvent::Set(), m_Cond.Wakeup error", 0);
		return CLStatus(-1, 0);
	}

	return CLStatus(0, 0);
}

CLStatus CLEvent::Wait()
{
	try
	{
		CLCriticalSection cs(&m_Mutex);

		while(m_pEventInfo->Flag == 0)
		{
			CLStatus s = m_Cond.Wait(&m_Mutex);
			if(!s.IsSuccess())
			{
				CLLogger::WriteLogMsg("In CLEvent::Wait(), m_Cond.Wait error", 0);
				return CLStatus(-1, 0);
			}
		}

		if(m_pEventInfo->bSemaphore != 0)
		{
			m_pEventInfo->Flag = m_pEventInfo->Flag - 1;
		}
		else
		{
			m_pEventInfo->Flag = 0;
		}
	}
	catch(const char* str)
	{
		CLLogger::WriteLogMsg("In CLEvent::Wait(), exception arise", 0);
		return CLStatus(-1, 0);
	}
	
	return CLStatus(0, 0);
}