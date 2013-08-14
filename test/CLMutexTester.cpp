#include <gtest/gtest.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "LibExecutive.h"

struct TestStructForCLMutex
{
	CLMutex *pmutex;
	int i;
};

static const int count = 1000000;

static void* TestThreadForCLMutex_MultiThread(void *arg)
{
	TestStructForCLMutex *pT = (TestStructForCLMutex *)arg;

	for(int i = 0; i < count; i++)
	{
		EXPECT_TRUE((pT->pmutex->Lock()).IsSuccess());

		pT->i = pT->i + 1;

		EXPECT_TRUE((pT->pmutex->Unlock()).IsSuccess());
	}
	
	return 0;
}

TEST(CLMutex, MultiThread)
{
	TestStructForCLMutex *pT = new TestStructForCLMutex;
	pT->pmutex = new CLMutex();
	pT->i = 0;
	
	pthread_t tid;
	pthread_create(&tid, 0, TestThreadForCLMutex_MultiThread, pT);

	TestThreadForCLMutex_MultiThread(pT);

	pthread_join(tid, 0);

	EXPECT_EQ(pT->i, 2*count);

	delete pT->pmutex;
	delete pT;
}

TEST(CLMutex, MultiThread_Pthread)
{
	pthread_mutex_t pmutex = PTHREAD_MUTEX_INITIALIZER;

	TestStructForCLMutex *pT = new TestStructForCLMutex;
	pT->pmutex = new CLMutex(&pmutex);
	pT->i = 0;

	pthread_t tid;
	pthread_create(&tid, 0, TestThreadForCLMutex_MultiThread, pT);

	TestThreadForCLMutex_MultiThread(pT);

	pthread_join(tid, 0);

	EXPECT_EQ(pT->i, 2*count);

	delete pT->pmutex;
	delete pT;
}

static void *thread_for_record_and_pthread(void *arg)
{
	TestStructForCLMutex *p = (TestStructForCLMutex *)arg;

	for(int j = 0; j < count; j++)
	{
		p->pmutex->Lock();

		lseek(p->i, SEEK_SET, 0);
		long k = 0;
		read(p->i, &k, sizeof(long));

		lseek(p->i, SEEK_SET, 0);
		k++;
		write(p->i, &k, sizeof(long));

		p->pmutex->Unlock();
	}

	return 0;
}

TEST(CLMutex, RecordLock)
{
	int fd = open("/tmp/test_for_record_lock", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	long i = 0;
	write(fd, &i, sizeof(long));

	pid_t pid = fork();
	if(pid == 0)
	{
		{
			CLMutex mutex("testforrecordlock", MUTEX_USE_RECORD_LOCK);

			TestStructForCLMutex sl;
			sl.i = fd;
			sl.pmutex = &mutex;

			thread_for_record_and_pthread(&sl);

			close(fd);
		}

		CLLibExecutiveInitializer::Destroy();

		_exit(0);
	}
	
	{
		CLMutex mutex("testforrecordlock", MUTEX_USE_RECORD_LOCK);

		TestStructForCLMutex sl;
		sl.i = fd;
		sl.pmutex = &mutex;

		thread_for_record_and_pthread(&sl);
	}
	
	waitpid(pid, 0, 0);

	long k = 0;
	lseek(fd, SEEK_SET, 0);
	read(fd, &k, sizeof(long));

	close(fd);

	EXPECT_EQ(k, 2*count);
}

TEST(CLMutex, RecordLockAndPthread)
{
	int fd = open("/tmp/test_for_record_lock_and_pthread", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	long i = 0;
	write(fd, &i, sizeof(long));

	pid_t pid = fork();
	if(pid == 0)
	{
		{
			CLMutex mutex("mutex_forrecord_and_pthread", MUTEX_USE_RECORD_LOCK_AND_PTHREAD);
			TestStructForCLMutex sl;
			sl.i = fd;
			sl.pmutex = &mutex;

			pthread_t tid;
			pthread_create(&tid, 0, thread_for_record_and_pthread, &sl);

			thread_for_record_and_pthread(&sl);

			pthread_join(tid, 0);

			close(fd);			
		}

		CLLibExecutiveInitializer::Destroy();

		_exit(0);
	}

	{
		CLMutex mutex("mutex_forrecord_and_pthread", MUTEX_USE_RECORD_LOCK_AND_PTHREAD);
		TestStructForCLMutex sl;
		sl.i = fd;
		sl.pmutex = &mutex;

		pthread_t tid;
		pthread_create(&tid, 0, thread_for_record_and_pthread, &sl);

		thread_for_record_and_pthread(&sl);

		pthread_join(tid, 0);
	}

	waitpid(pid, 0, 0);

	long k = 0;
	lseek(fd, SEEK_SET, 0);
	read(fd, &k, sizeof(long));

	close(fd);

	EXPECT_EQ(k, 4*count);
}

TEST(CLMutex, RecordLockAndPthread2)
{
	int fd = open("/tmp/test_for_record_lock_and_pthread2", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	long i = 0;
	write(fd, &i, sizeof(long));

	pthread_mutex_t pmutex = PTHREAD_MUTEX_INITIALIZER;

	pid_t pid = fork();
	if(pid == 0)
	{
		{
			CLMutex mutex("mutex_forrecord_and_pthread2", &pmutex);
			TestStructForCLMutex sl;
			sl.i = fd;
			sl.pmutex = &mutex;

			pthread_t tid;
			pthread_create(&tid, 0, thread_for_record_and_pthread, &sl);

			thread_for_record_and_pthread(&sl);

			pthread_join(tid, 0);

			close(fd);			
		}

		CLLibExecutiveInitializer::Destroy();

		_exit(0);
	}

	{
		CLMutex mutex("mutex_forrecord_and_pthread2", &pmutex);
		TestStructForCLMutex sl;
		sl.i = fd;
		sl.pmutex = &mutex;

		pthread_t tid;
		pthread_create(&tid, 0, thread_for_record_and_pthread, &sl);

		thread_for_record_and_pthread(&sl);

		pthread_join(tid, 0);
	}

	waitpid(pid, 0, 0);

	long k = 0;
	lseek(fd, SEEK_SET, 0);
	read(fd, &k, sizeof(long));

	close(fd);

	EXPECT_EQ(k, 4*count);
}

static void *thread_for_shared_mutex_bypthread(void *arg)
{
	long *pg = (long *)arg;

	for(int i = 0; i < count; i++)
	{
		CLMutex mutex("mutex_for_CLMutex_SharedMutexByPthread", MUTEX_USE_SHARED_PTHREAD);
		CLCriticalSection cs(&mutex);

		(*pg) = (*pg) + 1;
	}

	return 0;
}

TEST(CLMutex, SharedMutexByPthread)
{
	CLSharedMemory *psm = new CLSharedMemory("test_for_CLMutex_SharedMutexByPthread", 8);
	long *pg = (long *)(psm->GetAddress());
	*pg = 0;

	pid_t pid = fork();
	if(pid == 0)
	{
		pthread_t tid;
		pthread_create(&tid, 0, thread_for_shared_mutex_bypthread, pg);

		thread_for_shared_mutex_bypthread(pg);

		pthread_join(tid, 0);

		delete psm;

		CLLibExecutiveInitializer::Destroy();

		_exit(0);
	}

	pthread_t tid;
	pthread_create(&tid, 0, thread_for_shared_mutex_bypthread, pg);

	thread_for_shared_mutex_bypthread(pg);

	pthread_join(tid, 0);

	waitpid(pid, 0, 0);

	EXPECT_EQ(*pg, 4*count);

	delete psm;
}