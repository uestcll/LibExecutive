#include <gtest/gtest.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "LibExecutive.h"

using namespace std;

template<typename TSharedObjectItem>
static void ValidatePool(const char* SharedName)
{
	CLSharedMemory sm(SharedName);
	char *p = (char *)sm.GetAddress();

	int *pMagic = (int *)p;
	EXPECT_EQ(*pMagic, MAGIC_NUMBER_FOR_SHARED_OBJECT);

	TSharedObjectItem *pitem = (TSharedObjectItem *)(p + 4);

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


TEST(CLSharedObjectAllocator, CheckingForPool)
{
	CLLogger::WriteLogMsg("CLSharedObjectAllocator Test", 0);

	ValidatePool<SLSharedMutexItem>("shared_space_for_shared_mutex_allocator");
	ValidatePool<SLSharedEventItem>("shared_space_for_shared_event_allocator");
	ValidatePool<SLSharedConditionVariableItem>("shared_space_for_shared_condition_variable_allocator");
}

template<typename TSharedObjectAllocator>
static void TestForAllocate(const char* name)
{

	char *p1 = (char *)TSharedObjectAllocator::Get(name);
	SLSharedObjectHead *p2 = (SLSharedObjectHead *)(p1 - sizeof(SLSharedObjectHead));

	EXPECT_TRUE(p2->Status == ALLOCATED_SHARED_OBJECT);
	EXPECT_TRUE(p2->RefCount == 1);
	EXPECT_TRUE(strcmp(p2->strSharedObjectName, name) == 0);

	EXPECT_TRUE((TSharedObjectAllocator::Release(name).IsSuccess()));

	EXPECT_FALSE((TSharedObjectAllocator::Release(name).IsSuccess()));
}

TEST(CLSharedConditionVariableAllocator, CheckingForAllocate)
{
	TestForAllocate<CLSharedObjectAllocator<CLSharedMutexImpl,pthread_mutex_t> >("CV_For_CLSharedMutexAllocator_CheckingForAllocate");
	TestForAllocate<CLSharedObjectAllocator<CLSharedEventImpl,SLEventInfo> >("CV_For_CLSharedEventAllocator_CheckingForAllocate");
	TestForAllocate<CLSharedObjectAllocator<CLSharedConditionVariableImpl,pthread_cond_t> >("CV_For_CLSharedConditionVariableAllocator_CheckingForAllocate");
}

const int count = 1000000;

static void *thread_for_CLSharedMutexAllocator_MultiProcess_MuiltThread(void *arg)
{
	const char *mutex_name = "mutex_for_CLSharedMutexAllocator_MultiProcess";

	long *p = (long *)arg;

	pthread_mutex_t *pmutex = NULL;

	for(int i = 0; i < count; i++)
	{
		pmutex = CLSharedObjectAllocator<CLSharedMutexImpl,pthread_mutex_t>::Get(mutex_name);

		{
			CLMutex mutex(pmutex);
			CLCriticalSection cs(&mutex);

			(*p) = (*p) + 1;
		}

		EXPECT_TRUE((CLSharedObjectAllocator<CLSharedMutexImpl,pthread_mutex_t>::Release(mutex_name).IsSuccess()));
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

			pthread_cond_t *pCV = CLSharedObjectAllocator<CLSharedConditionVariableImpl,pthread_cond_t>::Get(name_cv);

			CLConditionVariable cv(pCV);

			EXPECT_TRUE((cv.Wakeup()).IsSuccess());

			EXPECT_TRUE((CLSharedObjectAllocator<CLSharedConditionVariableImpl,pthread_cond_t>::Release(name_cv).IsSuccess()));
		}

		delete psm;

		CLLibExecutiveInitializer::Destroy();

		_exit(0);
	}

	CLMutex mutex(name_mutex, MUTEX_USE_SHARED_PTHREAD);

	pthread_cond_t *pCV = CLSharedObjectAllocator<CLSharedConditionVariableImpl,pthread_cond_t>::Get(name_cv);

	CLConditionVariable cv(pCV);

	{
		CLCriticalSection cs(&mutex);

		while(*flag == 0)
			EXPECT_TRUE((cv.Wait(&mutex)).IsSuccess());
	}

	EXPECT_TRUE((CLSharedObjectAllocator<CLSharedConditionVariableImpl,pthread_cond_t>::Release(name_cv).IsSuccess()));

	EXPECT_EQ(*p, 5);

	waitpid(pid, 0, 0);

	delete psm;
}

TEST(CLSharedConditionVariableAllocator, CheckingForPoolAgain)
{
	ValidatePool<SLSharedMutexItem>("shared_space_for_shared_mutex_allocator");
	ValidatePool<SLSharedEventItem>("shared_space_for_shared_event_allocator");
	ValidatePool<SLSharedConditionVariableItem>("shared_space_for_shared_condition_variable_allocator");
}
