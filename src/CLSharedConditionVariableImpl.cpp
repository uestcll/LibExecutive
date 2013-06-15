#include "CLSharedConditionVariableImpl.h"
#include "CLLogger.h"

#define SHARED_SPACE_FOR_SHARED_CONDITION_VARIABLE_ALLOCATOR "shared_space_for_shared_condition_variable_allocator"

CLSharedConditionVariableImpl::CLSharedConditionVariableImpl()
{
	m_nItemSize = sizeof(SLSharedConditionVariableItem);
	m_strSharedSpaceName = SHARED_SPACE_FOR_SHARED_CONDITION_VARIABLE_ALLOCATOR;
}

CLSharedConditionVariableImpl::~CLSharedConditionVariableImpl()
{
}

CLStatus CLSharedConditionVariableImpl::InitializeSharedObject(SLSharedObjectHead *pObject)
{
	SLSharedConditionVariableItem *pCondItem = (SLSharedConditionVariableItem *)pObject;

	pthread_condattr_t attr;
	int r = pthread_condattr_init(&attr);
	if(r != 0)
	{
		CLLogger::WriteLogMsg("In CLSharedConditionVariableImpl::InitializeSharedObject(), pthread_condattr_init error", r);
		return CLStatus(-1, r);
	}

	r = pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	if(r != 0)
	{
		CLLogger::WriteLogMsg("In CLSharedConditionVariableImpl::InitializeSharedObject(), pthread_condattr_setpshared error", r);

		pthread_condattr_destroy(&attr);

		return CLStatus(-1, r);
	}

	r = pthread_cond_init(&(pCondItem->cond), &attr);
	if(r != 0)
	{
		CLLogger::WriteLogMsg("In CLSharedConditionVariableImpl::InitializeSharedObject(), pthread_cond_init error", r);

		pthread_condattr_destroy(&attr);

		return CLStatus(-1, r);
	}

	r = pthread_condattr_destroy(&attr);
	if(r != 0)
	{
		CLLogger::WriteLogMsg("In CLSharedConditionVariableImpl::InitializeSharedObject(), pthread_condattr_destroy error", r);
		return CLStatus(-1, r);
	}

	return CLStatus(0, 0);
}

CLStatus CLSharedConditionVariableImpl::DestroySharedObject(SLSharedObjectHead *pObject)
{
	SLSharedConditionVariableItem *pCondItem = (SLSharedConditionVariableItem *)pObject;

	int r = pthread_cond_destroy(&(pCondItem->cond));
	if(r != 0)
	{
		CLLogger::WriteLogMsg("In CLSharedConditionVariableImpl::DestroySharedObject(), pthread_cond_destroy error", r);
		return CLStatus(-1, r);
	}

	return CLStatus(0, 0);
}