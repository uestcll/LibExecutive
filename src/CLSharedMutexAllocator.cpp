#include <string.h>
#include "CLSharedMutexAllocator.h"
#include "CLLogger.h"
#include "CLCriticalSection.h"
#include "CLMutex.h"
#include "CLSharedMutexImpl.h"

#define MUTEX_FOR_SHARED_MUTEX_ALLOCATOR "mutex_for_shared_mutex_allocator"
pthread_mutex_t CLSharedMutexAllocator::m_MutexForSharedMutex = PTHREAD_MUTEX_INITIALIZER;

CLSharedMutexAllocator *CLSharedMutexAllocator::m_pAllocator = 0;

CLSharedMutexAllocator::CLSharedMutexAllocator()
{
	m_pImpl = new CLSharedMutexImpl;

	CLMutex mutex(MUTEX_FOR_SHARED_MUTEX_ALLOCATOR, &m_MutexForSharedMutex);
	CLCriticalSection cs(&mutex);

	if(!((m_pImpl->Initialize()).IsSuccess()))
		throw "In CLSharedMutexAllocator::CLSharedMutexAllocator(), m_pImpl->Initialize error";
}

CLSharedMutexAllocator::~CLSharedMutexAllocator()
{
	m_pImpl->Destroy();
	delete m_pImpl;
}

pthread_mutex_t* CLSharedMutexAllocator::Get(const char *pstrMutexName)
{
	CLSharedMutexAllocator *p = CLSharedMutexAllocator::GetInstance();
	if(p == 0)
		return 0;

	return p->GetSharedMutex(pstrMutexName);
}

CLStatus CLSharedMutexAllocator::Release(const char *pstrMutexName)
{
	CLSharedMutexAllocator *p = CLSharedMutexAllocator::GetInstance();
	if(p == 0)
		return CLStatus(-1, 0);

	return p->ReleaseSharedMutex(pstrMutexName);
}

CLSharedMutexAllocator* CLSharedMutexAllocator::GetInstance()
{
	return m_pAllocator;
}

CLStatus CLSharedMutexAllocator::Create()
{
	if(m_pAllocator == 0)
	{
		try
		{
			m_pAllocator = new CLSharedMutexAllocator();
		}
		catch(const char *str)
		{
			CLLogger::WriteLogMsg(str, 0);
			return CLStatus(-1, 0);
		}
	}

	return CLStatus(0, 0);
}

CLStatus CLSharedMutexAllocator::Destroy()
{
	if(m_pAllocator == 0)
		return CLStatus(0, 0);

	CLMutex mutex(MUTEX_FOR_SHARED_MUTEX_ALLOCATOR, &m_MutexForSharedMutex);
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

pthread_mutex_t *CLSharedMutexAllocator::GetSharedMutex(const char *pstrMutexName)
{
	if(pstrMutexName == 0)
		return 0;

	int len = strlen(pstrMutexName);
	if((len == 0) || (len >= LENGTH_OF_SHARED_OBJECT_NAME))
		return 0;

	CLMutex mutex(MUTEX_FOR_SHARED_MUTEX_ALLOCATOR, &m_MutexForSharedMutex);
	CLCriticalSection cs(&mutex);

	if(m_pAllocator == 0)
	{
		CLLogger::WriteLogMsg("In CLSharedMutexAllocator::GetSharedMutex(), m_pAllocator == 0", 0);
		return 0;
	}

	return (pthread_mutex_t *)m_pImpl->GetSharedObject(pstrMutexName);
}

CLStatus CLSharedMutexAllocator::ReleaseSharedMutex(const char *pstrMutexName)
{
	if(pstrMutexName == 0)
		return CLStatus(-1, 0);

	int len = strlen(pstrMutexName);
	if((len == 0) || (len >= LENGTH_OF_SHARED_OBJECT_NAME))
		return CLStatus(-1, 0);

	CLMutex mutex(MUTEX_FOR_SHARED_MUTEX_ALLOCATOR, &m_MutexForSharedMutex);
	CLCriticalSection cs(&mutex);

	if(m_pAllocator == 0)
	{
		CLLogger::WriteLogMsg("In CLSharedMutexAllocator::ReleaseSharedMutex(), m_pAllocator == 0", 0);
		return CLStatus(-1, 0);
	}

	return m_pImpl->ReleaseSharedObject(pstrMutexName);
}