#include <memory.h>
#include <string.h>
#include "CLSharedObjectsImpl.h"
#include "CLSharedMemory.h"
#include "CLLogger.h"
#include "CLCriticalSection.h"
#include "CLMutex.h"

CLSharedObjectsImpl::CLSharedObjectsImpl()
{
}

CLSharedObjectsImpl::~CLSharedObjectsImpl()
{
}

CLStatus CLSharedObjectsImpl::Initialize()
{
	m_pSharedMemory = new CLSharedMemory(m_strSharedSpaceName.c_str(), sizeof(int) + NUMBER_OF_SHARED_OBJECT * m_nItemSize);

	int *pMagicNumber = (int *)(m_pSharedMemory->GetAddress());

	m_pSharedObjects = (SLSharedObjectHead *)((char *)pMagicNumber + sizeof(int));

	if((*pMagicNumber) == MAGIC_NUMBER_FOR_SHARED_OBJECT)
		return CLStatus(0, 0);

	bool bSuccess = true;
	for(int i = 0; i < NUMBER_OF_SHARED_OBJECT; i++)
	{
		SLSharedObjectHead *pItem = (SLSharedObjectHead *)((char *)m_pSharedObjects + i * m_nItemSize);
		if(pItem->Status == UNINITIALIZED_SHARED_OBJECT)
		{
			if(InitializeSharedObject(pItem).IsSuccess())
			{
				pItem->Status = INITIALIZED_SHARED_OBJECT;
				pItem->RefCount = 0;
				memset(pItem->strSharedObjectName, 0, LENGTH_OF_SHARED_OBJECT_NAME);
			}
			else
				bSuccess = false;
		}
	}

	if(bSuccess)
		*pMagicNumber = MAGIC_NUMBER_FOR_SHARED_OBJECT;

	return CLStatus(0, 0);
}

CLStatus CLSharedObjectsImpl::Destroy()
{
	delete m_pSharedMemory;
	return CLStatus(0, 0);
}

void *CLSharedObjectsImpl::GetSharedObject(const char *pstrSharedObjectName)
{
	int empty_index = -1;
	bool bfind_empty = false;

	for(int i = 0; i < NUMBER_OF_SHARED_OBJECT; i++)
	{
		SLSharedObjectHead *pItem = (SLSharedObjectHead *)((char *)m_pSharedObjects + i * m_nItemSize);

		if(pItem->Status == UNINITIALIZED_SHARED_OBJECT)
			continue;

		if(pItem->Status == INITIALIZED_SHARED_OBJECT)
		{
			if(bfind_empty)
				continue;

			empty_index = i;
			bfind_empty = true;

			continue;
		}

		if(pItem->Status == ALLOCATED_SHARED_OBJECT)
		{
			if(strcmp(pstrSharedObjectName, pItem->strSharedObjectName) == 0)
			{
				pItem->RefCount++;

				return (char *)pItem + sizeof(SLSharedObjectHead);
			}
		}
	}

	if(empty_index != -1)
	{
		SLSharedObjectHead *pItem = (SLSharedObjectHead *)((char *)m_pSharedObjects + empty_index * m_nItemSize);

		pItem->Status = ALLOCATED_SHARED_OBJECT;
		pItem->RefCount = 1;

		strcpy(pItem->strSharedObjectName, pstrSharedObjectName);

		return (char *)pItem + sizeof(SLSharedObjectHead);
	}
	else
	{
		CLLogger::WriteLogMsg("In CLSharedObjectsImpl::GetSharedObject(), shared memory is full", 0);
		return 0;
	}
}

CLStatus CLSharedObjectsImpl::ReleaseSharedObject(const char *pstrSharedObjectName)
{
	for(int i = 0; i < NUMBER_OF_SHARED_OBJECT; i++)
	{
		SLSharedObjectHead *pItem = (SLSharedObjectHead *)((char *)m_pSharedObjects + i * m_nItemSize);

		if(pItem->Status == ALLOCATED_SHARED_OBJECT)
		{
			if(strcmp(pstrSharedObjectName, pItem->strSharedObjectName) == 0)
			{
				pItem->RefCount--;

				if(pItem->RefCount == 0)
				{
					DestroySharedObject(pItem);

					pItem->Status = UNINITIALIZED_SHARED_OBJECT;
					memset(pItem->strSharedObjectName, 0, LENGTH_OF_SHARED_OBJECT_NAME);

					if(InitializeSharedObject(pItem).IsSuccess())
					{
						pItem->Status = INITIALIZED_SHARED_OBJECT;
					}
				}

				return CLStatus(0, 0);
			}
		}
	}

	return CLStatus(-1, 0);
}