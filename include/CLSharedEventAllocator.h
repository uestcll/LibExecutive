#ifndef CLSharedEventAllocator_H
#define CLSharedEventAllocator_H

#include "CLStatus.h"

struct SLEventInfo;
class CLSharedEventImpl;

class CLSharedEventAllocator
{
public:
	static CLSharedEventAllocator* GetInstance();

	static SLEventInfo* Get(const char *pstrEventName);
	static CLStatus Release(const char *pstrEventName);

	SLEventInfo *GetSharedEventInfo(const char *pstrEventName);
	CLStatus ReleaseEventInfo(const char *pstrEventName);

	friend class CLLibExecutiveInitializer;

private:
	static CLStatus Create();
	static CLStatus Destroy();

private:
	CLSharedEventAllocator();
	virtual ~CLSharedEventAllocator();

	CLSharedEventAllocator(const CLSharedEventAllocator&);
	CLSharedEventAllocator& operator=(const CLSharedEventAllocator&);

private:
	static CLSharedEventAllocator *m_pAllocator;
	CLSharedEventImpl *m_pImpl;
};

#endif