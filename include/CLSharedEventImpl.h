#ifndef CLSharedEventImpl_H
#define CLSharedEventImpl_H

#include "CLSharedObjectsImpl.h"

template<typename T, typename SL> class CLSharedObjectAllocator;

struct SLEventInfo
{
	long Flag;
	long bSemaphore;
	long Context;
};

struct SLSharedEventItem
{
	SLSharedObjectHead head;
	SLEventInfo EventInfo;
};

class CLSharedEventImpl : public CLSharedObjectsImpl
{
public:
	virtual CLStatus InitializeSharedObject(SLSharedObjectHead *pObject);
	virtual CLStatus DestroySharedObject(SLSharedObjectHead *pObject);

	friend class CLSharedObjectAllocator<CLSharedEventImpl,SLEventInfo>;

private:
	CLSharedEventImpl();
	virtual ~CLSharedEventImpl();
	
	CLSharedEventImpl(const CLSharedEventImpl&);
	CLSharedEventImpl& operator=(const CLSharedEventImpl&);
};

#endif