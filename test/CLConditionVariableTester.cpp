#include <gtest/gtest.h>
#include <pthread.h>
#include <unistd.h>
#include "LibExecutive.h"

struct TestForCLConditionVariable
{
	CLMutex mutex;
	CLConditionVariable cond;
	volatile int flag;
	int i;
};

void* TestThreadForCLConditionVariable(void *arg)
{
	TestForCLConditionVariable *pT = (TestForCLConditionVariable *)arg;
	
	EXPECT_TRUE((pT->mutex.Lock()).IsSuccess());

	pT->flag = 1;

	pT->i  = 5;
	
	EXPECT_TRUE((pT->mutex.Unlock()).IsSuccess());

	EXPECT_TRUE((pT->cond.Wakeup()).IsSuccess());
	
	return 0;
}

TEST(CLConditionVariable, Normal)
{
	TestForCLConditionVariable *pT = new TestForCLConditionVariable();
	pT->flag = 0;
	pT->i = 0;
	
	pthread_t tid;	
	pthread_create(&tid, 0, TestThreadForCLConditionVariable, pT);
	
	EXPECT_TRUE((pT->mutex.Lock()).IsSuccess());
	while(pT->flag == 0)
		EXPECT_TRUE((pT->cond.Wait(&(pT->mutex))).IsSuccess());
	EXPECT_TRUE((pT->mutex.Unlock()).IsSuccess());

	EXPECT_EQ(pT->i, 5);

	pthread_join(tid, 0);

	delete pT;
}

TEST(CLConditionVariable, Wait_null)
{
	CLConditionVariable cv;
	EXPECT_FALSE(cv.Wait(0).IsSuccess());
}

static pthread_cond_t g_cond = PTHREAD_COND_INITIALIZER;
volatile int g_flag_for_cond = 0;
CLMutex g_mutex_for_cond;
int g_i_for_cond = 0;

void* TestThreadForCLConditionVariable2(void *arg)
{
	sleep(2);
	{
		CLCriticalSection cs(&g_mutex_for_cond);

		g_flag_for_cond = 1;

		g_i_for_cond = 5;
	}
	CLConditionVariable cv(&g_cond);

	EXPECT_TRUE((cv.Wakeup()).IsSuccess());

	return 0;
}

TEST(CLConditionVariable, Normal2)
{
	pthread_t tid;	
	pthread_create(&tid, 0, TestThreadForCLConditionVariable2, 0);
	
	CLConditionVariable cv(&g_cond);

	{
		CLCriticalSection cs(&g_mutex_for_cond);

		while(g_flag_for_cond == 0)
			EXPECT_TRUE((cv.Wait(&g_mutex_for_cond)).IsSuccess());
	}

	EXPECT_EQ(g_i_for_cond, 5);

	pthread_join(tid, 0);
}

static pthread_cond_t g_cond2 = PTHREAD_COND_INITIALIZER;
volatile int g_flag_for_cond2 = 0;
int g_i_for_cond2 = 0;

void* TestThreadForCLConditionVariable3(void *arg)
{
	CLMutex g_mutex_for_cond2("test_for_CLConditionVariable_shared_mutex2", MUTEX_USE_SHARED_PTHREAD);

	sleep(2);
	{
		CLCriticalSection cs(&g_mutex_for_cond2);

		g_flag_for_cond2 = 1;

		g_i_for_cond2 = 5;
	}
	CLConditionVariable cv(&g_cond2);

	EXPECT_TRUE((cv.Wakeup()).IsSuccess());

	return 0;
}

TEST(CLConditionVariable, Normal_Shared_mutex)
{
	pthread_t tid;	
	pthread_create(&tid, 0, TestThreadForCLConditionVariable3, 0);

	CLMutex g_mutex_for_cond2("test_for_CLConditionVariable_shared_mutex2", MUTEX_USE_SHARED_PTHREAD);

	CLConditionVariable cv(&g_cond2);

	{
		CLCriticalSection cs(&g_mutex_for_cond2);

		while(g_flag_for_cond2 == 0)
			EXPECT_TRUE((cv.Wait(&g_mutex_for_cond2)).IsSuccess());
	}

	EXPECT_EQ(g_i_for_cond2, 5);

	pthread_join(tid, 0);
}

TEST(CLConditionVariable, Multi_process_for_Shared_Cond)
{
	CLSharedMemory sm("test_for_multi_process_for_shared_cond", 16);
	long *p = (long *)sm.GetAddress();
	*p = 0;

	long *flag = (long *)(((char *)p) + 8);
	*flag = 0;

	pid_t pid = fork();
	if(pid == 0)
	{
		sleep(2);

		{
			CLMutex mutex("mutex_for_test_for_multi_process_for_shared_cond", MUTEX_USE_SHARED_PTHREAD);

			{
				CLCriticalSection cs(&mutex);

				*flag = 1;

				*p = 5;
			}

			CLConditionVariable cv("test_conditoin_variable_for_multiprocess");

			EXPECT_TRUE((cv.Wakeup()).IsSuccess());
		}

		exit(0);
	}

	CLMutex mutex("mutex_for_test_for_multi_process_for_shared_cond", MUTEX_USE_SHARED_PTHREAD);

	CLConditionVariable cv("test_conditoin_variable_for_multiprocess");

	{
		CLCriticalSection cs(&mutex);

		while(*flag == 0)
			EXPECT_TRUE((cv.Wait(&mutex)).IsSuccess());
	}

	EXPECT_EQ(*p, 5);

	waitpid(pid, 0, 0);
}