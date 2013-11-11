#ifndef CLSharedObjectAllocator_H
#define CLSharedObjectAllocator_H

#include <pthread.h>
#include <string.h>
#include "CLMutex.h"
#include "CLCriticalSection.h"
#include "CLSharedMutexImpl.h"
#include "CLSharedConditionVariableImpl.h"
#include "CLSharedEventImpl.h"
#include "CLLogger.h"
#include "CLStatus.h"
#include "DefinitionForConst.h"

template<typename TSharedObjectPool, typename TSharedObject>
class CLSharedObjectAllocator
{
public:
	static CLSharedObjectAllocator* GetInstance();

	static TSharedObject* Get(const char *pstrEventName);
	static CLStatus Release(const char *pstrEventName);

	TSharedObject *GetSharedObject(const char *pstrObjectName);
	CLStatus ReleaseSharedObject(const char *pstrObjectName);

	friend class CLLibExecutiveInitializer;

private:
	static CLStatus Create();
	static CLStatus Destroy();

	static CLMutex* GetMutex();

private:
	CLSharedObjectAllocator();
	~CLSharedObjectAllocator();

	CLSharedObjectAllocator(const CLSharedObjectAllocator&);
	CLSharedObjectAllocator& operator=(const CLSharedObjectAllocator&);

private:
	static CLSharedObjectAllocator *m_pAllocator;
	static pthread_mutex_t m_MutexForSharedMutex;
	TSharedObjectPool *m_pImpl;
};

template<typename TSharedObjectPool, typename TSharedObject>
CLSharedObjectAllocator<TSharedObjectPool, TSharedObject>* CLSharedObjectAllocator<TSharedObjectPool,TSharedObject>::m_pAllocator = 0;

template<typename TSharedObjectPool, typename TSharedObject>
pthread_mutex_t CLSharedObjectAllocator<TSharedObjectPool, TSharedObject>::m_MutexForSharedMutex = PTHREAD_MUTEX_INITIALIZER;

template<>
inline CLMutex* CLSharedObjectAllocator<CLSharedMutexImpl, pthread_mutex_t>::GetMutex()
{
	CLMutex* mutex = new CLMutex(MUTEX_FOR_SHARED_MUTEX_ALLOCATOR, &m_MutexForSharedMutex);
	return mutex;
}

template<>
inline CLMutex* CLSharedObjectAllocator<CLSharedConditionVariableImpl, pthread_cond_t>::GetMutex()
{
	CLMutex* mutex = new CLMutex(MUTEX_FOR_SHARED_CONDITION_VARIABLE_ALLOCATOR, MUTEX_USE_SHARED_PTHREAD);
	return mutex;
}

template<>
inline CLMutex* CLSharedObjectAllocator<CLSharedEventImpl, SLEventInfo>::GetMutex()
{
	CLMutex* mutex = new CLMutex(MUTEX_FOR_SHARED_EVENT_ALLOCATOR, MUTEX_USE_SHARED_PTHREAD);
	return mutex;
}

template<typename TSharedObjectPool, typename TSharedObject>
CLSharedObjectAllocator<TSharedObjectPool, TSharedObject>::CLSharedObjectAllocator()
{
	m_pImpl = new TSharedObjectPool;

	bool isReturn = false;

	CLMutex* mutex = GetMutex();

	{
		CLCriticalSection cs(mutex);
		CLStatus s = m_pImpl->Initialize();
		if(!s.IsSuccess())
			isReturn = true;
	}

	delete mutex;

	if(isReturn)
	{
		CLLogger::WriteLogMsg("In CLSharedEventAllocator::CLSharedEventAllocator(), m_pImpl->Initialize error", 0);
		throw "In CLSharedEventAllocator::CLSharedEventAllocator(), m_pImpl->Initialize error";
	}
}

template<typename TSharedObjectPool, typename TSharedObject>
CLSharedObjectAllocator<TSharedObjectPool, TSharedObject>::~CLSharedObjectAllocator()
{
	CLStatus s = m_pImpl->Destroy();
	if(!s.IsSuccess())
		CLLogger::WriteLogMsg("In CLSharedEventAllocator::~CLSharedEventAllocator(), m_pImpl->Destroy error", 0);

	delete m_pImpl;
}

template<typename TSharedObjectPool, typename TSharedObject>
TSharedObject* CLSharedObjectAllocator<TSharedObjectPool, TSharedObject>::Get(const char *pstrEventName)
{
	CLSharedObjectAllocator *p = CLSharedObjectAllocator::GetInstance();
	if(p == 0)
		return 0;

	return p->GetSharedObject(pstrEventName);
}

template<typename TSharedObjectPool, typename TSharedObject>
CLStatus CLSharedObjectAllocator<TSharedObjectPool, TSharedObject>::Release(const char *pstrEventName)
{
	CLSharedObjectAllocator *p = CLSharedObjectAllocator::GetInstance();
	if(p == 0)
		return CLStatus(-1, 0);

	return p->ReleaseSharedObject(pstrEventName);
}

template<typename TSharedObjectPool, typename TSharedObject>
CLSharedObjectAllocator<TSharedObjectPool, TSharedObject>* CLSharedObjectAllocator<TSharedObjectPool,TSharedObject>::GetInstance()
{
	return m_pAllocator;
}

template<typename TSharedObjectPool, typename TSharedObject>
CLStatus CLSharedObjectAllocator<TSharedObjectPool, TSharedObject>::Create()
{
	if(m_pAllocator == 0)
	{
		try
		{   
			m_pAllocator = new CLSharedObjectAllocator();
		}
		catch(const char *str)
		{
			CLLogger::WriteLogMsg(str, 0);
			return CLStatus(-1, 0);
		} 
	}
	return CLStatus(0, 0);
}

template<typename TSharedObjectPool, typename TSharedObject>
CLStatus CLSharedObjectAllocator<TSharedObjectPool, TSharedObject>::Destroy()
{
	if(m_pAllocator == 0)
		return CLStatus(0, 0);

	bool isReturn = false;
	CLMutex* mutex = GetMutex();

	{
		CLCriticalSection cs(mutex);

		try
		{
			delete m_pAllocator;
		}
		catch(const char* str)
		{
			m_pAllocator = 0;

			CLLogger::WriteLogMsg(str, 0);
			isReturn = true;
		}
	}

	delete mutex;

	if(isReturn)
		return CLStatus(-1, 0);

	m_pAllocator = 0;

	return CLStatus(0, 0);
}

template<typename TSharedObjectPool, typename TSharedObject>
TSharedObject *CLSharedObjectAllocator<TSharedObjectPool, TSharedObject>::GetSharedObject(const char *pstrObjectName)
{
	if(pstrObjectName == 0)
		return 0;

	int len = strlen(pstrObjectName);
	if((len == 0) || (len >= LENGTH_OF_SHARED_OBJECT_NAME))
		return 0;

	bool isReturn = false;
	TSharedObject* sob = 0;
	CLMutex* mutex = GetMutex();

	{
		CLCriticalSection cs(mutex);
		if(m_pAllocator == 0)
		{
			CLLogger::WriteLogMsg("In CLSharedObjectAllocator::GetSharedObject(), m_pAllocator == 0", 0);
			isReturn = true;
		}
		else
			sob = (TSharedObject *)m_pImpl->GetSharedObject(pstrObjectName);
	}

	delete mutex;

	if(isReturn)
		return 0;

	return sob;
}

template<typename TSharedObjectPool, typename TSharedObject>
CLStatus CLSharedObjectAllocator<TSharedObjectPool, TSharedObject>::ReleaseSharedObject(const char *pstrObjectName)
{
	if(pstrObjectName == 0)
		return CLStatus(-1, 0);

	int len = strlen(pstrObjectName);
	if((len == 0) || (len >= LENGTH_OF_SHARED_OBJECT_NAME))
		return CLStatus(-1, 0);

	bool isReturn = false;
	long m_ReturnCode = 0;
	long m_ErrorCode = 0;

	CLMutex* mutex = GetMutex();

	{
		CLCriticalSection cs(mutex);
		if(m_pAllocator == 0)
		{
			CLLogger::WriteLogMsg("In CLSharedEventAllocator::ReleaseSharedEvent(), m_pAllocator == 0", 0);
			isReturn = true;
		}
		else
		{
			CLStatus s1 = m_pImpl->ReleaseSharedObject(pstrObjectName);
			m_ReturnCode = s1.m_clReturnCode;
			m_ErrorCode = s1.m_clErrorCode;
		}
	}

	delete mutex;

	if(isReturn)
		return CLStatus(-1, 0);

	return CLStatus(m_ReturnCode, m_ErrorCode);
}

#endif