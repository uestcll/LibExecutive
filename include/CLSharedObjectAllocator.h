#ifndef CLSHAREDOBJECTALLOCATOR_H
#define CLSHAREDOBJECTALLOCATOR_H

#include "CLStatus.h"
#include <string.h>
#include "CLLogger.h"
#include "CLCriticalSection.h"
#include "CLMutex.h"
#include <pthread.h>
#include "CLSharedMutexImpl.h"
#include "CLSharedEventImpl.h"
#include "CLSharedConditionVariableImpl.h"
//#include "CLSharedObjectsImpl.h"
#include <iostream>
#include <errno.h>


#define MUTEX_FOR_SHARED_CONDITION_VARIABLE_ALLOCATOR "mutex_for_shared_condition_variable_allocator"
#define MUTEX_FOR_SHARED_MUTEX_ALLOCATOR "mutex_for_shared_mutex_allocator"
#define MUTEX_FOR_SHARED_EVENT_ALLOCATOR "mutex_for_shared_event_allocator"

class CLSharedMutexImpl;
class CLSharedEventImpl;
class CLSharedConditionVariableImpl;
//struct SLEventInfo;

template<typename TSharedObjectPool,typename TSharedObject>
class CLSharedObjectAllocator
{
public:

	static CLSharedObjectAllocator<TSharedObjectPool,TSharedObject> *GetInstance()
	{
		return m_pAllocator;
	}

	static TSharedObject* Get(const char *pstrEventName)
	{
		CLSharedObjectAllocator *p = CLSharedObjectAllocator::GetInstance();
		if(p == 0)
			return 0;
		return p->GetSharedObject(pstrEventName);
	}

	static CLStatus Release(const char *pstrEventName)
	{
		CLSharedObjectAllocator *p = CLSharedObjectAllocator::GetInstance();
		if(p == 0)
			return CLStatus(-1, 0);
		return p->ReleaseSharedObject(pstrEventName);
	}

	TSharedObject *GetSharedObject(const char *pstrEventName)
	{
		if(pstrEventName == 0)
			return 0;
		int len = strlen(pstrEventName);
		if((len == 0) || (len >= LENGTH_OF_SHARED_OBJECT_NAME))
			return 0;
		/*	CLMutex mutex(MUTEX_FOR_SHARED_EVENT_ALLOCATOR, MUTEX_USE_SHARED_PTHREAD); */
		bool isReturn = false;
		TSharedObject* sob = 0;
		CLMutex* mutex = CLSharedObjectAllocator::GetMutex();
		{
			CLCriticalSection cs(mutex);
			if(m_pAllocator == 0)
			{
				CLLogger::WriteLogMsg("In CLSharedEventAllocator::GetSharedEvent(), m_pAllocator == 0", 0);
				isReturn = true;
			}
			else
			sob = (TSharedObject *)m_pImpl->GetSharedObject(pstrEventName);
		}

		delete mutex;
		if(isReturn)
			return 0;
		return sob;
	}

	CLStatus ReleaseSharedObject(const char *pstrEventName)
	{
		if(pstrEventName == 0)
			return CLStatus(-1, 0);
		int len = strlen(pstrEventName);
		if((len == 0) || (len >= LENGTH_OF_SHARED_OBJECT_NAME))
			return CLStatus(-1, 0);
		/*	CLMutex mutex(MUTEX_FOR_SHARED_EVENT_ALLOCATOR, MUTEX_USE_SHARED_PTHREAD); */
		bool isReturn = false;
		long m_ReturnCode = 0;
		long m_ErrorCode = 0;
		CLMutex* mutex = CLSharedObjectAllocator::GetMutex();
		{
			CLCriticalSection cs(mutex);
			if(m_pAllocator == 0)
			{
				CLLogger::WriteLogMsg("In CLSharedEventAllocator::ReleaseSharedEvent(), m_pAllocator == 0", 0);
				isReturn = true;
			}
			else
			{
				CLStatus s1 = m_pImpl->ReleaseSharedObject(pstrEventName);
				m_ReturnCode = s1.m_clReturnCode;
				m_ErrorCode = s1.m_clErrorCode;
			}
		}
		
		delete mutex;
		if(isReturn)
			return CLStatus(-1, 0);
		return CLStatus(m_ReturnCode,m_ErrorCode);
	}

	friend class CLLibExecutiveInitializer;

#ifndef DEBUG_TEST

private:

#endif

	static CLStatus Create()
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

	static CLStatus Destroy()
	{
		if(m_pAllocator == 0)
			return CLStatus(0, 0);
		/*	CLMutex mutex(MUTEX_FOR_SHARED_EVENT_ALLOCATOR, MUTEX_USE_SHARED_PTHREAD); */
		bool isReturn = false;
		CLMutex* mutex = CLSharedObjectAllocator::GetMutex();
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

	static CLMutex* GetMutex();

private:

	CLSharedObjectAllocator()
	{
		m_pImpl = new TSharedObjectPool;
		/*	CLMutex mutex(MUTEX_FOR_SHARED_EVENT_ALLOCATOR, MUTEX_USE_SHARED_PTHREAD); */
		bool isReturn = false;
		CLMutex* mutex = CLSharedObjectAllocator::GetMutex();
		{
			CLCriticalSection cs(mutex);
			CLStatus s = m_pImpl->Initialize();
			if(!s.IsSuccess())
			{
					  std::cout<<s.m_clReturnCode<<"ess"<<errno<<std::endl;
				isReturn = true;
				
			}
		}
		delete mutex;
		if (isReturn)
			throw "In CLSharedEventAllocator::CLSharedEventAllocator(), m_pImpl->Initialize error";
		
	
	}

	virtual ~CLSharedObjectAllocator()
	{
		if(!((m_pImpl->Destroy()).IsSuccess()))
		{
			delete m_pImpl;
			throw "In CLSharedEventAllocator::~CLSharedEventAllocator(), m_pImpl->Destroy error";
		}
		delete m_pImpl;
	}

	CLSharedObjectAllocator(const CLSharedObjectAllocator&);
	CLSharedObjectAllocator& operator=(const CLSharedObjectAllocator&);

private:
	static CLSharedObjectAllocator<TSharedObjectPool,TSharedObject> *m_pAllocator;
	static pthread_mutex_t m_MutexForSharedMutex;
	TSharedObjectPool *m_pImpl;
};

template<typename TSharedObjectPool,typename TSharedObject>
CLSharedObjectAllocator<TSharedObjectPool,TSharedObject> *CLSharedObjectAllocator<TSharedObjectPool,TSharedObject>::m_pAllocator = 0;

template<typename TSharedObjectPool,typename TSharedObject>
pthread_mutex_t CLSharedObjectAllocator<TSharedObjectPool,TSharedObject>::m_MutexForSharedMutex = PTHREAD_MUTEX_INITIALIZER;

template<>
inline CLMutex* CLSharedObjectAllocator<CLSharedMutexImpl,pthread_mutex_t>::GetMutex()
{
CLMutex* mutex = new CLMutex(MUTEX_FOR_SHARED_MUTEX_ALLOCATOR,&m_MutexForSharedMutex);
	return mutex;
}

template<>
inline CLMutex* CLSharedObjectAllocator<CLSharedConditionVariableImpl,pthread_cond_t>::GetMutex(){
CLMutex* mutex = new CLMutex(MUTEX_FOR_SHARED_CONDITION_VARIABLE_ALLOCATOR,MUTEX_USE_SHARED_PTHREAD);
	return mutex;
}

template<>
inline CLMutex* CLSharedObjectAllocator<CLSharedEventImpl,SLEventInfo>::GetMutex()
{
CLMutex* mutex = new CLMutex(MUTEX_FOR_SHARED_EVENT_ALLOCATOR,MUTEX_USE_SHARED_PTHREAD);
	return mutex;
} 

#endif
