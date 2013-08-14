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
	CLSharedMemory sm("shared_space_for_shared_mutex_allocator");
	char *p = (char *)sm.GetAddress();

	int *pMagic = (int *)p;
	EXPECT_EQ(*pMagic, MAGIC_NUMBER_FOR_SHARED_OBJECT);

	SLSharedMutexItem *pitem = (SLSharedMutexItem *)(p + 4);

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

TEST(CLSharedMutexAllocator, CheckingForPool)
{
	ValidatePool();
}

TEST(CLSharedMutexAllocator, CheckingForAllocate)
{
	const char *name = "Mutex_For_CLSharedMutexAllocator_CheckingForAllocate";
	
	char *p1 = (char *)CLSharedMutexAllocator::Get(name);
	SLSharedObjectHead *p2 = (SLSharedObjectHead *)(p1 - sizeof(SLSharedObjectHead));
	
	EXPECT_TRUE(p2->Status == ALLOCATED_SHARED_OBJECT);
	EXPECT_TRUE(p2->RefCount == 1);
	EXPECT_TRUE(strcmp(p2->strSharedObjectName, name) == 0);

	EXPECT_TRUE(CLSharedMutexAllocator::Release(name).IsSuccess());

	EXPECT_FALSE(CLSharedMutexAllocator::Release(name).IsSuccess());
}

const int count = 1000000;

static void *thread_for_CLSharedMutexAllocator_MultiProcess_MuiltThread(void *arg)
{
	const char *mutex_name = "mutex_for_CLSharedMutexAllocator_MultiProcess";

	long *p = (long *)arg;

	pthread_mutex_t *pmutex = NULL;

	for(int i = 0; i < count; i++)
	{
		pmutex = CLSharedMutexAllocator::Get(mutex_name);

		{
			CLMutex mutex(pmutex);
			CLCriticalSection cs(&mutex);

			(*p) = (*p) + 1;
		}

		EXPECT_TRUE(CLSharedMutexAllocator::Release(mutex_name).IsSuccess());
	}

	return 0;
}

TEST(CLSharedMutexAllocator, MultiProcess)
{
	const char *shared_name = "shared_resource_for_CLSharedMutexAllocator_MultiProcess";

	pid_t pid = fork();
	if(pid == 0)
	{
		{
			CLSharedMemory sm(shared_name, sizeof(long));
			long *p = (long *)sm.GetAddress();

			pthread_t tid;
			pthread_create(&tid, 0, thread_for_CLSharedMutexAllocator_MultiProcess_MuiltThread, p);

			thread_for_CLSharedMutexAllocator_MultiProcess_MuiltThread(p);

			pthread_join(tid, 0);
		}

		CLLibExecutiveInitializer::Destroy();

		_exit(0);
	}

	CLSharedMemory sm(shared_name, sizeof(long));
	long *p = (long *)sm.GetAddress();

	pthread_t tid;
	pthread_create(&tid, 0, thread_for_CLSharedMutexAllocator_MultiProcess_MuiltThread, p);

	thread_for_CLSharedMutexAllocator_MultiProcess_MuiltThread(p);

	pthread_join(tid, 0);

	waitpid(pid, 0, 0);

	EXPECT_EQ(*p, 4 * count);
}

TEST(CLSharedMutexAllocator, CheckingForPoolAgain)
{
	ValidatePool();
}