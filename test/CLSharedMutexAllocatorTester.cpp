#include <gtest/gtest.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "LibExecutive.h"

using namespace std;

struct TEST_SLSharedMutex
{
	int Status;
	int RefCount;
	char strMutexName[256];
	pthread_mutex_t mutex;
};

TEST(CLSharedMutexAllocator, Verify)
{
	CLSharedMemory sm("shared_space_for_shared_mutex_allocator");
	char *p = (char *)sm.GetAddress();
	
	int *pMagic = (int *)p;
	EXPECT_EQ(*pMagic, 0x12345678);

	TEST_SLSharedMutex *pitem = (TEST_SLSharedMutex *)(p + 4);

	for(int i = 0; i < 1024; i++)
	{
		int status = pitem[i].Status;
		EXPECT_TRUE((status == 1) || (status == 2));

		if(status == 1)
		{
			EXPECT_EQ(pitem[i].RefCount, 0);
			EXPECT_TRUE(pitem[i].strMutexName[0] == 0);
		}
		else
		{
			EXPECT_TRUE(pitem[i].strMutexName[0] != 0);
			EXPECT_GE(pitem[i].RefCount, 0);
		}
	}
}

TEST(CLSharedMutexAllocator, DataVerfiy)
{
	char *pTmp = (char *)CLSharedMutexAllocator::Get("mutex_CLSharedMutexAllocator_datavrify_sum");
	char *pHead = pTmp - 268;
	EXPECT_EQ(*((int *)pHead), 0x12345678);
	EXPECT_TRUE(CLSharedMutexAllocator::Release("mutex_CLSharedMutexAllocator_datavrify_sum").IsSuccess());

	for(int i = 0; i < 1024; i++)
	{
		string str = "mutex_CLSharedMutexAllocator_datavrify_";
		char buf[20];
		snprintf(buf, 20, "%d", i);
		str += buf;

		char *p = (char *)CLSharedMutexAllocator::Get(str.c_str());
		EXPECT_EQ(p - pHead, 4 + 264 + (40 + 264) * i);
	}

	for(int i = 0; i < 1024; i++)
	{
		string str = "mutex_CLSharedMutexAllocator_datavrify_";
		char buf[20];
		snprintf(buf, 20, "%d", i);
		str += buf;

		EXPECT_TRUE(CLSharedMutexAllocator::Release(str.c_str()).IsSuccess());
	}

	for(int i = 0; i < 1024; i++)
	{
		char *p = pHead + 4 + (264 + 40) * i;
		
		int *pStatus = (int *)p;
		int *pRef = (int *)(p + 4);
		char *pName = p + 8;

		EXPECT_EQ(*pStatus, 1);
		EXPECT_EQ(*pRef, 0);

		for(int j = 0; j < 256; j++)
		{
			EXPECT_EQ(pName[j], 0);
		}
	}

	char *q908 = 0;

	for(int i = 0; i < 1024; i++)
	{
		string str = "mutex_CLSharedMutexAllocator_datavrify_";
		char buf[20];
		snprintf(buf, 20, "%d", i);
		str += buf;

		char *p = (char *)CLSharedMutexAllocator::Get(str.c_str());

		if(i == 908)
			q908 = p;

		EXPECT_EQ(p - pHead, 4 + 264 + (40 + 264) * i);
	}

	{
		string str = "mutex_CLSharedMutexAllocator_datavrify_908";
		char *p = (char *)CLSharedMutexAllocator::Get(str.c_str());
		EXPECT_EQ(p, q908);
		EXPECT_EQ(*((int *)(p-256-4)), 2);

		EXPECT_TRUE(CLSharedMutexAllocator::Release(str.c_str()).IsSuccess());
	}

	for(int i = 0; i < 1024; i++)
	{
		if(i % 3 == 2)
		{
			string str = "mutex_CLSharedMutexAllocator_datavrify_";
			char buf[20];
			snprintf(buf, 20, "%d", i);
			str += buf;

			EXPECT_TRUE(CLSharedMutexAllocator::Release(str.c_str()).IsSuccess());
		}
	}

	{
		string str = "mutex_CLSharedMutexAllocator_datavrify_2";
		char *p = (char *)CLSharedMutexAllocator::Get(str.c_str());

		EXPECT_EQ(*((int *)(p-256-4)), 1);

		EXPECT_EQ(p - pHead, 4 + 264 + (40 + 264) * 2);

		EXPECT_TRUE(CLSharedMutexAllocator::Release(str.c_str()).IsSuccess());
	}

	for(int i = 0; i < 1024; i++)
	{
		if(i % 3 != 2)
		{
			string str = "mutex_CLSharedMutexAllocator_datavrify_";
			char buf[20];
			snprintf(buf, 20, "%d", i);
			str += buf;

			EXPECT_TRUE(CLSharedMutexAllocator::Release(str.c_str()).IsSuccess());
		}
	}

	for(int i = 0; i < 1024; i++)
	{
		char *p = pHead + 4 + (264 + 40) * i;

		int *pStatus = (int *)p;
		int *pRef = (int *)(p + 4);
		char *pName = p + 8;

		EXPECT_EQ(*pStatus, 1);
		EXPECT_EQ(*pRef, 0);

		for(int j = 0; j < 256; j++)
		{
			EXPECT_EQ(pName[j], 0);
		}
	}
}

TEST(CLSharedMutexAllocator, Normal)
{
	pid_t pid = fork();
	if(pid == 0)
	{
		CLSharedMemory sm("test_for_shared_mutex_normal", sizeof(long));
		long *p = (long *)sm.GetAddress();

		CLSharedMutexAllocator *palloc = CLSharedMutexAllocator::GetInstance();
		EXPECT_TRUE(palloc != 0);

		pthread_mutex_t *pmutex = palloc->GetSharedMutex("shared_mutex_for_test_shared_mutex_alloc_normal");

		for(int i = 0; i < 1000000; i++)
		{
			CLMutex mutex(pmutex);
			CLCriticalSection cs(&mutex);

			(*p) = (*p) + 1;
		}

		EXPECT_TRUE(palloc->ReleaseSharedMutex("shared_mutex_for_test_shared_mutex_alloc_normal").IsSuccess());

		_exit(0);
	}

	CLSharedMemory sm("test_for_shared_mutex_normal", sizeof(long));
	long *p = (long *)sm.GetAddress();
	
	CLSharedMutexAllocator *palloc = CLSharedMutexAllocator::GetInstance();
	EXPECT_TRUE(palloc != 0);

	pthread_mutex_t *pmutex = palloc->GetSharedMutex("shared_mutex_for_test_shared_mutex_alloc_normal");

	for(int i = 0; i < 1000000; i++)
	{
		CLMutex mutex(pmutex);
		CLCriticalSection cs(&mutex);

		(*p) = (*p) + 1;
	}

	EXPECT_TRUE(palloc->ReleaseSharedMutex("shared_mutex_for_test_shared_mutex_alloc_normal").IsSuccess());

	waitpid(pid, 0, 0);

	EXPECT_EQ(*p, 2000000);
}

void *thread_for_CLSharedMutexAllocator_Normal_Thread(void *arg)
{
	long *p = (long *)arg;

	CLSharedMutexAllocator *palloc = CLSharedMutexAllocator::GetInstance();
	EXPECT_TRUE(palloc != 0);

	pthread_mutex_t *pmutex = palloc->GetSharedMutex("shared_mutex_for_test_shared_mutex_alloc_normal_thread");

	for(int i = 0; i < 1000000; i++)
	{
		CLMutex mutex(pmutex);
		CLCriticalSection cs(&mutex);

		(*p) = (*p) + 1;
	}

	EXPECT_TRUE(palloc->ReleaseSharedMutex("shared_mutex_for_test_shared_mutex_alloc_normal_thread").IsSuccess());

	return 0;
}

TEST(CLSharedMutexAllocator, Normal_Thread)
{
	pid_t pid = fork();
	if(pid == 0)
	{
		CLSharedMemory sm("test_for_shared_mutex_normal_thread", sizeof(long));
		long *p = (long *)sm.GetAddress();

		pthread_t tid;
		pthread_create(&tid, 0, thread_for_CLSharedMutexAllocator_Normal_Thread, p);

		CLSharedMutexAllocator *palloc = CLSharedMutexAllocator::GetInstance();
		EXPECT_TRUE(palloc != 0);

		pthread_mutex_t *pmutex = palloc->GetSharedMutex("shared_mutex_for_test_shared_mutex_alloc_normal_thread");

		for(int i = 0; i < 1000000; i++)
		{
			CLMutex mutex(pmutex);
			CLCriticalSection cs(&mutex);

			(*p) = (*p) + 1;
		}

		EXPECT_TRUE(palloc->ReleaseSharedMutex("shared_mutex_for_test_shared_mutex_alloc_normal_thread").IsSuccess());

		pthread_join(tid, 0);

		_exit(0);
	}

	CLSharedMemory sm("test_for_shared_mutex_normal_thread", sizeof(long));
	long *p = (long *)sm.GetAddress();

	pthread_t tid;
	pthread_create(&tid, 0, thread_for_CLSharedMutexAllocator_Normal_Thread, p);

	CLSharedMutexAllocator *palloc = CLSharedMutexAllocator::GetInstance();
	EXPECT_TRUE(palloc != 0);

	pthread_mutex_t *pmutex = palloc->GetSharedMutex("shared_mutex_for_test_shared_mutex_alloc_normal_thread");

	for(int i = 0; i < 1000000; i++)
	{
		CLMutex mutex(pmutex);
		CLCriticalSection cs(&mutex);

		(*p) = (*p) + 1;
	}

	EXPECT_TRUE(palloc->ReleaseSharedMutex("shared_mutex_for_test_shared_mutex_alloc_normal_thread").IsSuccess());

	pthread_join(tid, 0);

	waitpid(pid, 0, 0);

	EXPECT_EQ(*p, 4000000);
}

void *multithread__allocate_mutex(void *)
{
	for(int i = 0; i < 1000000; i++)
	{
		EXPECT_TRUE(CLSharedMutexAllocator::Get("multithread_multiprocess_allocate_mutex_mutex") != 0);
		EXPECT_TRUE(CLSharedMutexAllocator::Release("multithread_multiprocess_allocate_mutex_mutex").IsSuccess());
	}
}

TEST(CLSharedMutexAllocator, multithread_multiprocess_allocate_mutex)
{
	pid_t pid = fork();
	if(pid == 0)
	{
		pthread_t tid;
		pthread_create(&tid, 0, multithread__allocate_mutex, 0);

		for(int i = 0; i < 1000000; i++)
		{
			EXPECT_TRUE(CLSharedMutexAllocator::Get("multithread_multiprocess_allocate_mutex_mutex") != 0);
			EXPECT_TRUE(CLSharedMutexAllocator::Release("multithread_multiprocess_allocate_mutex_mutex").IsSuccess());
		}

		pthread_join(tid, 0);
		_exit(0);
	}

	pthread_t tid;
	pthread_create(&tid, 0, multithread__allocate_mutex, 0);

	for(int i = 0; i < 1000000; i++)
	{
		char *pTmp = (char *)CLSharedMutexAllocator::Get("multithread_multiprocess_allocate_mutex_mutex");
		EXPECT_TRUE(pTmp != 0);

		char *pHead = pTmp - 268;
		EXPECT_EQ(*((int *)pHead), 0x12345678);

		int status = *((int *)(pHead + 4));
		EXPECT_EQ(status, 2);

		int cref = *((int *)(pHead + 8));
		EXPECT_TRUE((cref == 1) || (cref == 2) || (cref == 3) || (cref == 4));

		EXPECT_TRUE(CLSharedMutexAllocator::Release("multithread_multiprocess_allocate_mutex_mutex").IsSuccess());
	}

	pthread_join(tid, 0);
	waitpid(pid, 0, 0);

	char *pTmp = (char *)CLSharedMutexAllocator::Get("mutex_CLSharedMutexAllocator_datavrify_sum");
	char *pHead = pTmp - 268;
	EXPECT_EQ(*((int *)pHead), 0x12345678);
	EXPECT_TRUE(CLSharedMutexAllocator::Release("mutex_CLSharedMutexAllocator_datavrify_sum").IsSuccess());

	for(int i = 0; i < 1024; i++)
	{
		char *p = pHead + 4 + (264 + 40) * i;

		int *pStatus = (int *)p;
		int *pRef = (int *)(p + 4);
		char *pName = p + 8;

		EXPECT_EQ(*pStatus, 1);
		EXPECT_EQ(*pRef, 0);

		for(int j = 0; j < 256; j++)
		{
			EXPECT_EQ(pName[j], 0);
		}
	}
}

TEST(CLSharedMutexAllocator, Exec_Normal)
{
	pid_t pid = fork();
	EXPECT_TRUE(pid != -1);

	if(pid == 0)
	{
		execl("../test_for_exec/test_for_CLSharedMutexAllocator_Exec_Normal/main", "../test_for_exec/test_for_CLSharedMutexAllocator_Exec_Normal/main", (char *)0);
		EXPECT_TRUE(false);
		_exit(0);
	}

	CLSharedMemory sm("test_for_shared_mutex_Exec", sizeof(long));
	long *p = (long *)sm.GetAddress();

	CLSharedMutexAllocator *palloc = CLSharedMutexAllocator::GetInstance();
	EXPECT_TRUE(palloc != 0);

	pthread_mutex_t *pmutex = palloc->GetSharedMutex("shared_mutex_for_test_shared_mutex_alloc_Exec");

	for(int i = 0; i < 1000000; i++)
	{
		CLMutex mutex(pmutex);
		CLCriticalSection cs(&mutex);

		(*p) = (*p) + 1;
	}

	EXPECT_TRUE(palloc->ReleaseSharedMutex("shared_mutex_for_test_shared_mutex_alloc_Exec").IsSuccess());

	waitpid(pid, 0, 0);

	EXPECT_EQ(*p, 2000000);
}

TEST(CLSharedMutexAllocator, Exec_Mutex)
{
	wait(NULL);
	pid_t pid = fork();
	EXPECT_TRUE(pid != -1);

	if(pid == 0)
	{
		execl("../test_for_exec/test_for_CLSharedMutexAllocator_Exec_Mutex/main", "../test_for_exec/test_for_CLSharedMutexAllocator_Exec_Mutex/main", (char *)0);
		EXPECT_TRUE(false);
		_exit(0);
	}

	sleep(10);

	for(int i = 0; i < 1000000; i++)
	{
		char *pTmp = (char *)CLSharedMutexAllocator::Get("multithread_multiprocess_allocate_mutex_mutex");
		EXPECT_TRUE(pTmp != 0);

		char *pHead = pTmp - 268;
		EXPECT_EQ(*((int *)pHead), 0x12345678);

		int status = *((int *)(pHead + 4));
		EXPECT_EQ(status, 2);

		int cref = *((int *)(pHead + 8));
		EXPECT_TRUE((cref == 1) || (cref == 2));

		EXPECT_TRUE(CLSharedMutexAllocator::Release("multithread_multiprocess_allocate_mutex_mutex").IsSuccess());
	}

	waitpid(pid, 0, 0);

	char *pTmp = (char *)CLSharedMutexAllocator::Get("mutex_CLSharedMutexAllocator_datavrify_sum");
	char *pHead = pTmp - 268;
	EXPECT_EQ(*((int *)pHead), 0x12345678);
	EXPECT_TRUE(CLSharedMutexAllocator::Release("mutex_CLSharedMutexAllocator_datavrify_sum").IsSuccess());

	for(int i = 0; i < 1024; i++)
	{
		char *p = pHead + 4 + (264 + 40) * i;

		int *pStatus = (int *)p;
		int *pRef = (int *)(p + 4);
		char *pName = p + 8;

		EXPECT_EQ(*pStatus, 1);
		EXPECT_EQ(*pRef, 0);

		for(int j = 0; j < 256; j++)
		{
			EXPECT_EQ(pName[j], 0);
		}
	}
}