#ifndef CLSharedMutexAllocator_H
#define CLSharedMutexAllocator_H

#include <pthread.h>
#include "CLStatus.h"

class CLSharedMutexImpl;

class CLSharedMutexAllocator
{
public:
	static CLSharedMutexAllocator* GetInstance();

	static pthread_mutex_t* Get(const char *pstrMutexName);
	static CLStatus Release(const char *pstrMutexName);

	pthread_mutex_t *GetSharedMutex(const char *pstrMutexName);
	CLStatus ReleaseSharedMutex(const char *pstrMutexName);

	friend class CLLibExecutiveInitializer;
	
private:
	static CLStatus Create();
	static CLStatus Destroy();

private:
	CLSharedMutexAllocator();
	virtual ~CLSharedMutexAllocator();

	CLSharedMutexAllocator(const CLSharedMutexAllocator&);
	CLSharedMutexAllocator& operator=(const CLSharedMutexAllocator&);

private:
	static CLSharedMutexAllocator *m_pAllocator;
	static pthread_mutex_t m_MutexForSharedMutex;
	CLSharedMutexImpl *m_pImpl;
};

#endif