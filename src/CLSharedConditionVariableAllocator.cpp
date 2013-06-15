#include <string.h>
#include "CLSharedConditionVariableAllocator.h"
#include "CLLogger.h"
#include "CLCriticalSection.h"
#include "CLMutex.h"
#include "CLSharedConditionVariableImpl.h"

#define MUTEX_FOR_SHARED_CONDITION_VARIABLE_ALLOCATOR "mutex_for_shared_condition_variable_allocator"

CLSharedConditionVariableAllocator *CLSharedConditionVariableAllocator::m_pAllocator = 0;

CLSharedConditionVariableAllocator::CLSharedConditionVariableAllocator()
{
	m_pImpl = new CLSharedConditionVariableImpl;

	CLMutex mutex(MUTEX_FOR_SHARED_CONDITION_VARIABLE_ALLOCATOR, MUTEX_USE_SHARED_PTHREAD);
	CLCriticalSection cs(&mutex);

	if(!((m_pImpl->Initialize()).IsSuccess()))
		throw "In CLSharedConditionVariableAllocator::CLSharedConditionVariableAllocator(), m_pImpl->Initialize error";
}

CLSharedConditionVariableAllocator::~CLSharedConditionVariableAllocator()
{
	m_pImpl->Destroy();
	delete m_pImpl;
}

pthread_cond_t* CLSharedConditionVariableAllocator::Get(const char *pstrCondName)
{
	CLSharedConditionVariableAllocator *p = CLSharedConditionVariableAllocator::GetInstance();
	if(p == 0)
		return 0;

	return p->GetSharedCond(pstrCondName);
}

CLStatus CLSharedConditionVariableAllocator::Release(const char *pstrCondName)
{
	CLSharedConditionVariableAllocator *p = CLSharedConditionVariableAllocator::GetInstance();
	if(p == 0)
		return CLStatus(-1, 0);

	return p->ReleaseSharedCond(pstrCondName);
}

CLSharedConditionVariableAllocator* CLSharedConditionVariableAllocator::GetInstance()
{
	return m_pAllocator;
}

CLStatus CLSharedConditionVariableAllocator::Create()
{
	if(m_pAllocator == 0)
	{
		try
		{
			m_pAllocator = new CLSharedConditionVariableAllocator();
		}
		catch(const char *str)
		{
			CLLogger::WriteLogMsg(str, 0);
			return CLStatus(-1, 0);
		}
	}

	return CLStatus(0, 0);
}

CLStatus CLSharedConditionVariableAllocator::Destroy()
{
	if(m_pAllocator == 0)
		return CLStatus(0, 0);

	CLMutex mutex(MUTEX_FOR_SHARED_CONDITION_VARIABLE_ALLOCATOR, MUTEX_USE_SHARED_PTHREAD);
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

pthread_cond_t *CLSharedConditionVariableAllocator::GetSharedCond(const char *pstrCondName)
{
	if(pstrCondName == 0)
		return 0;

	int len = strlen(pstrCondName);
	if((len == 0) || (len >= LENGTH_OF_SHARED_OBJECT_NAME))
		return 0;

	CLMutex mutex(MUTEX_FOR_SHARED_CONDITION_VARIABLE_ALLOCATOR, MUTEX_USE_SHARED_PTHREAD);
	CLCriticalSection cs(&mutex);

	if(m_pAllocator == 0)
	{
		CLLogger::WriteLogMsg("In CLSharedConditionVariableAllocator::GetSharedCond(), m_pAllocator == 0", 0);
		return 0;
	}

	return (pthread_cond_t *)m_pImpl->GetSharedObject(pstrCondName);
}

CLStatus CLSharedConditionVariableAllocator::ReleaseSharedCond(const char *pstrCondName)
{
	if(pstrCondName == 0)
		return CLStatus(-1, 0);

	int len = strlen(pstrCondName);
	if((len == 0) || (len >= LENGTH_OF_SHARED_OBJECT_NAME))
		return CLStatus(-1, 0);

	CLMutex mutex(MUTEX_FOR_SHARED_CONDITION_VARIABLE_ALLOCATOR, MUTEX_USE_SHARED_PTHREAD);
	CLCriticalSection cs(&mutex);

	if(m_pAllocator == 0)
	{
		CLLogger::WriteLogMsg("In CLSharedConditionVariableAllocator::ReleaseSharedCond(), m_pAllocator == 0", 0);
		return CLStatus(-1, 0);
	}

	return m_pImpl->ReleaseSharedObject(pstrCondName);
}