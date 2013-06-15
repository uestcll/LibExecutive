#ifndef CLSharedConditionVariableAllocator_H
#define CLSharedConditionVariableAllocator_H

#include <pthread.h>
#include "CLStatus.h"

class CLSharedConditionVariableImpl;

class CLSharedConditionVariableAllocator
{
public:
	static CLSharedConditionVariableAllocator* GetInstance();

	static pthread_cond_t* Get(const char *pstrCondName);
	static CLStatus Release(const char *pstrCondName);

	pthread_cond_t *GetSharedCond(const char *pstrCondName);
	CLStatus ReleaseSharedCond(const char *pstrCondName);

	friend class CLLibExecutiveInitializer;

private:
	static CLStatus Create();
	static CLStatus Destroy();

private:
	CLSharedConditionVariableAllocator();
	virtual ~CLSharedConditionVariableAllocator();

	CLSharedConditionVariableAllocator(const CLSharedConditionVariableAllocator&);
	CLSharedConditionVariableAllocator& operator=(const CLSharedConditionVariableAllocator&);

private:
	static CLSharedConditionVariableAllocator *m_pAllocator;
	CLSharedConditionVariableImpl *m_pImpl;
};

#endif