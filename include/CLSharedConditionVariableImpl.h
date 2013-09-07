#ifndef CLSharedConditionVariableImpl_H
#define CLSharedConditionVariableImpl_H

#include <pthread.h>
#include "CLSharedObjectsImpl.h"

template<typename T,typename SL> class CLSharedObjectAllocator;

struct SLSharedConditionVariableItem
{
	SLSharedObjectHead head;
	pthread_cond_t cond;
};

class CLSharedConditionVariableImpl : public CLSharedObjectsImpl
{
public:
	virtual CLStatus InitializeSharedObject(SLSharedObjectHead *pObject);
	virtual CLStatus DestroySharedObject(SLSharedObjectHead *pObject);

//	friend  class CLSharedConditionVariableAllocator;
	friend  class CLSharedObjectAllocator<CLSharedConditionVariableImpl,pthread_cond_t>;

private:	
	CLSharedConditionVariableImpl();
	virtual ~CLSharedConditionVariableImpl();
	CLSharedConditionVariableImpl(const CLSharedConditionVariableImpl&);
	CLSharedConditionVariableImpl& operator=(const CLSharedConditionVariableImpl&);
};

#endif
