#ifndef CLSharedMutexImpl_H
#define CLSharedMutexImpl_H

#include <pthread.h>
#include "CLSharedObjectsImpl.h"
//#include "CLSharedObjectAllocator.h"

template<typename T,typename SL> class CLSharedObjectAllocator;
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

	//template<typename T,typename SL> friend class CLSharedObjectAllocator;
	friend class CLSharedObjectAllocator<CLSharedMutexImpl,pthread_mutex_t>;
//这里改动了没有使用友原类 


private:
	CLSharedMutexImpl();
	virtual ~CLSharedMutexImpl();
	CLSharedMutexImpl(const CLSharedMutexImpl&);
	CLSharedMutexImpl& operator=(const CLSharedMutexImpl&);
};

#endif
