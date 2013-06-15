#ifndef CLSharedEventImpl_H
#define CLSharedEventImpl_H

#include "CLSharedObjectsImpl.h"

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

	friend  class CLSharedEventAllocator;

private:
	CLSharedEventImpl();
	virtual ~CLSharedEventImpl();

	CLSharedEventImpl(const CLSharedEventImpl&);
	CLSharedEventImpl& operator=(const CLSharedEventImpl&);
};

#endif