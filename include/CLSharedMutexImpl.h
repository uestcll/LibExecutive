#ifndef CLSharedMutexImpl_H
#define CLSharedMutexImpl_H

#include <pthread.h>
#include "CLSharedObjectsImpl.h"

struct SLSharedMutexItem
{
	SLSharedObjectHead head;
	pthread_mutex_t mutex;
};

class CLSharedMutexImpl : public CLSharedObjectsImpl
{
public:
	virtual CLStatus InitializeSharedObject(SLSharedObjectHead *pObject);
	virtual CLStatus DestroySharedObject(SLSharedObjectHead *pObject);

	friend  class CLSharedMutexAllocator;

private:
	CLSharedMutexImpl();
	virtual ~CLSharedMutexImpl();

	CLSharedMutexImpl(const CLSharedMutexImpl&);
	CLSharedMutexImpl& operator=(const CLSharedMutexImpl&);
};

#endif