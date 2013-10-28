#ifndef CLSharedMutexImpl_H
#define CLSharedMutexImpl_H

#include <pthread.h>
#include "CLSharedObjectsImpl.h"

template<typename T, typename SL> class CLSharedObjectAllocator;

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

	friend class CLSharedObjectAllocator<CLSharedMutexImpl, pthread_mutex_t>;

private:
	CLSharedMutexImpl();
	virtual ~CLSharedMutexImpl();

	CLSharedMutexImpl(const CLSharedMutexImpl&);
	CLSharedMutexImpl& operator=(const CLSharedMutexImpl&);
};

#endif