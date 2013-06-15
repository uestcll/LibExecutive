#include <string.h>
#include "CLSharedEventAllocator.h"
#include "CLLogger.h"
#include "CLCriticalSection.h"
#include "CLMutex.h"
#include "CLSharedEventImpl.h"

#define MUTEX_FOR_SHARED_EVENT_ALLOCATOR "mutex_for_shared_event_allocator"

CLSharedEventAllocator *CLSharedEventAllocator::m_pAllocator = 0;

CLSharedEventAllocator::CLSharedEventAllocator()
{
	m_pImpl = new CLSharedEventImpl;

	CLMutex mutex(MUTEX_FOR_SHARED_EVENT_ALLOCATOR, MUTEX_USE_SHARED_PTHREAD);
	CLCriticalSection cs(&mutex);

	if(!((m_pImpl->Initialize()).IsSuccess()))
		throw "In CLSharedEventAllocator::CLSharedEventAllocator(), m_pImpl->Initialize error";
}

CLSharedEventAllocator::~CLSharedEventAllocator()
{
	m_pImpl->Destroy();
	delete m_pImpl;
}

SLEventInfo* CLSharedEventAllocator::Get(const char *pstrEventName)
{
	CLSharedEventAllocator *p = CLSharedEventAllocator::GetInstance();
	if(p == 0)
		return 0;

	return p->GetSharedEventInfo(pstrEventName);
}

CLStatus CLSharedEventAllocator::Release(const char *pstrEventName)
{
	CLSharedEventAllocator *p = CLSharedEventAllocator::GetInstance();
	if(p == 0)
		return CLStatus(-1, 0);

	return p->ReleaseEventInfo(pstrEventName);
}

CLSharedEventAllocator* CLSharedEventAllocator::GetInstance()
{
	return m_pAllocator;
}

CLStatus CLSharedEventAllocator::Create()
{
	if(m_pAllocator == 0)
	{
		try
		{
			m_pAllocator = new CLSharedEventAllocator();
		}
		catch(const char *str)
		{
			CLLogger::WriteLogMsg(str, 0);
			return CLStatus(-1, 0);
		}
	}

	return CLStatus(0, 0);
}

CLStatus CLSharedEventAllocator::Destroy()
{
	if(m_pAllocator == 0)
		return CLStatus(0, 0);

	CLMutex mutex(MUTEX_FOR_SHARED_EVENT_ALLOCATOR, MUTEX_USE_SHARED_PTHREAD);
	CLCriticalSection cs(&mutex);

	try
	{
		delete m_pAllocator;
	}
	catch(const char* str)
	{
		m_pAllocator = 0;

		CLLogger::WriteLogMsg(str, 0);
		return CLStatus(-1, 0);
	}

	m_pAllocator = 0;

	return CLStatus(0, 0);
}

SLEventInfo *CLSharedEventAllocator::GetSharedEventInfo(const char *pstrEventName)
{
	if(pstrEventName == 0)
		return 0;

	int len = strlen(pstrEventName);
	if((len == 0) || (len >= LENGTH_OF_SHARED_OBJECT_NAME))
		return 0;

	CLMutex mutex(MUTEX_FOR_SHARED_EVENT_ALLOCATOR, MUTEX_USE_SHARED_PTHREAD);
	CLCriticalSection cs(&mutex);

	if(m_pAllocator == 0)
	{
		CLLogger::WriteLogMsg("In CLSharedEventAllocator::GetSharedEvent(), m_pAllocator == 0", 0);
		return 0;
	}

	return (SLEventInfo *)m_pImpl->GetSharedObject(pstrEventName);
}

CLStatus CLSharedEventAllocator::ReleaseEventInfo(const char *pstrEventName)
{
	if(pstrEventName == 0)
		return CLStatus(-1, 0);

	int len = strlen(pstrEventName);
	if((len == 0) || (len >= LENGTH_OF_SHARED_OBJECT_NAME))
		return CLStatus(-1, 0);

	CLMutex mutex(MUTEX_FOR_SHARED_EVENT_ALLOCATOR, MUTEX_USE_SHARED_PTHREAD);
	CLCriticalSection cs(&mutex);

	if(m_pAllocator == 0)
	{
		CLLogger::WriteLogMsg("In CLSharedEventAllocator::ReleaseSharedEvent(), m_pAllocator == 0", 0);
		return CLStatus(-1, 0);
	}

	return m_pImpl->ReleaseSharedObject(pstrEventName);
}