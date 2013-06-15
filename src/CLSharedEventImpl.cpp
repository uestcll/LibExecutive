#include "CLSharedEventImpl.h"

#define SHARED_SPACE_FOR_SHARED_EVENT_ALLOCATOR "shared_space_for_shared_event_allocator"

CLSharedEventImpl::CLSharedEventImpl()
{
	m_nItemSize = sizeof(SLSharedEventItem);
	m_strSharedSpaceName = SHARED_SPACE_FOR_SHARED_EVENT_ALLOCATOR;
}

CLSharedEventImpl::~CLSharedEventImpl()
{
}

CLStatus CLSharedEventImpl::InitializeSharedObject(SLSharedObjectHead *pObject)
{
	SLSharedEventItem *pEventItem = (SLSharedEventItem *)pObject;

	pEventItem->EventInfo.Flag = 0;
	pEventItem->EventInfo.bSemaphore = 0;
	pEventItem->EventInfo.Context = 0;

	return CLStatus(0, 0);
}

CLStatus CLSharedEventImpl::DestroySharedObject(SLSharedObjectHead *pObject)
{
	SLSharedEventItem *pEventItem = (SLSharedEventItem *)pObject;
	
	pEventItem->EventInfo.Flag = 0;
	pEventItem->EventInfo.bSemaphore = 0;
	pEventItem->EventInfo.Context = 0;

	return CLStatus(0, 0);
}