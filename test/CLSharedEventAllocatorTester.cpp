#include <gtest/gtest.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "LibExecutive.h"

using namespace std;

static void ValidatePool()
{
	CLSharedMemory sm("shared_space_for_shared_event_allocator");
	char *p = (char *)sm.GetAddress();

	int *pMagic = (int *)p;
	EXPECT_EQ(*pMagic, MAGIC_NUMBER_FOR_SHARED_OBJECT);

	SLSharedEventItem *pitem = (SLSharedEventItem *)(p + 4);

	for(int i = 0; i < NUMBER_OF_SHARED_OBJECT; i++)
	{
		int status = pitem[i].head.Status;
		EXPECT_TRUE((status == INITIALIZED_SHARED_OBJECT) || (status == ALLOCATED_SHARED_OBJECT));

		if(status == INITIALIZED_SHARED_OBJECT)
		{
			EXPECT_EQ(pitem[i].head.RefCount, 0);
			EXPECT_TRUE(pitem[i].head.strSharedObjectName[0] == 0);
		}
		else
		{
			EXPECT_TRUE(pitem[i].head.strSharedObjectName[0] != 0);
			EXPECT_TRUE(pitem[i].head.RefCount > 0);
		}
	}
}

TEST(CLSharedEventAllocator, CheckingForPool)
{
	ValidatePool();
}

TEST(CLSharedEventAllocator, CheckingForAllocate)
{
	const char *name = "Event_For_CLSharedEventAllocator_CheckingForAllocate";
	
	char *p1 = (char *)CLSharedEventAllocator::Get(name);
	SLSharedObjectHead *p2 = (SLSharedObjectHead *)(p1 - sizeof(SLSharedObjectHead));
	
	EXPECT_TRUE(p2->Status == ALLOCATED_SHARED_OBJECT);
	EXPECT_TRUE(p2->RefCount == 1);
	EXPECT_TRUE(strcmp(p2->strSharedObjectName, name) == 0);

	EXPECT_TRUE(CLSharedEventAllocator::Release(name).IsSuccess());

	EXPECT_FALSE(CLSharedEventAllocator::Release(name).IsSuccess());
}