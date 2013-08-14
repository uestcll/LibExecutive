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
	CLSharedMemory sm("shared_space_for_shared_condition_variable_allocator");
	char *p = (char *)sm.GetAddress();

	int *pMagic = (int *)p;
	EXPECT_EQ(*pMagic, MAGIC_NUMBER_FOR_SHARED_OBJECT);

	SLSharedConditionVariableItem *pitem = (SLSharedConditionVariableItem *)(p + 4);

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

TEST(CLSharedConditionVariableAllocator, CheckingForPool)
{
	ValidatePool();
}

TEST(CLSharedConditionVariableAllocator, CheckingForAllocate)
{
	const char *name = "CV_For_CLSharedConditionVariableAllocator_CheckingForAllocate";
	
	char *p1 = (char *)CLSharedConditionVariableAllocator::Get(name);
	SLSharedObjectHead *p2 = (SLSharedObjectHead *)(p1 - sizeof(SLSharedObjectHead));
	
	EXPECT_TRUE(p2->Status == ALLOCATED_SHARED_OBJECT);
	EXPECT_TRUE(p2->RefCount == 1);
	EXPECT_TRUE(strcmp(p2->strSharedObjectName, name) == 0);

	EXPECT_TRUE(CLSharedConditionVariableAllocator::Release(name).IsSuccess());

	EXPECT_FALSE(CLSharedConditionVariableAllocator::Release(name).IsSuccess());
}

TEST(CLSharedConditionVariableAllocator, MultiProcess)
{
	const char *name_cv = "test_conditoin_variable_for_CLSharedConditionVariableAllocator_MultiProcess";
	const char *name_mutex = "mutex_for_test_for_CLSharedConditionVariableAllocator_MultiProcess";

	CLSharedMemory *psm = new CLSharedMemory("test_for_CLSharedConditionVariableAllocator_MultiProcess", 16);
	long *p = (long *)(psm->GetAddress());
	*p = 0;

	long *flag = (long *)(((char *)p) + 8);
	*flag = 0;

	pid_t pid = fork();
	if(pid == 0)
	{
		sleep(2);

		{
			CLMutex mutex(name_mutex, MUTEX_USE_SHARED_PTHREAD);

			{
				CLCriticalSection cs(&mutex);

				*flag = 1;

				*p = 5;
			}

			pthread_cond_t *pCV = CLSharedConditionVariableAllocator::Get(name_cv);

			CLConditionVariable cv(pCV);

			EXPECT_TRUE((cv.Wakeup()).IsSuccess());

			EXPECT_TRUE(CLSharedConditionVariableAllocator::Release(name_cv).IsSuccess());
		}

		delete psm;

		CLLibExecutiveInitializer::Destroy();

		_exit(0);
	}

	CLMutex mutex(name_mutex, MUTEX_USE_SHARED_PTHREAD);

	pthread_cond_t *pCV = CLSharedConditionVariableAllocator::Get(name_cv);

	CLConditionVariable cv(pCV);

	{
		CLCriticalSection cs(&mutex);

		while(*flag == 0)
			EXPECT_TRUE((cv.Wait(&mutex)).IsSuccess());
	}

	EXPECT_TRUE(CLSharedConditionVariableAllocator::Release(name_cv).IsSuccess());

	EXPECT_EQ(*p, 5);

	waitpid(pid, 0, 0);

	delete psm;
}

TEST(CLSharedConditionVariableAllocator, CheckingForPoolAgain)
{
	ValidatePool();
}