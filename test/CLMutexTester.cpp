#include <gtest/gtest.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "LibExecutive.h"

struct TestForCLMutex
{
	CLMutex mutex;
	int i;
};

void* TestThreadForCLMutex(void *arg)
{
	TestForCLMutex *pT = (TestForCLMutex *)arg;

	for(int i = 0; i < 1000000; i++)
	{
		EXPECT_TRUE((pT->mutex.Lock()).IsSuccess());
		pT->i = pT->i + 1;
		EXPECT_TRUE((pT->mutex.Unlock()).IsSuccess());
	}
	
	return 0;
}

TEST(CLMutex, Normal)
{
	TestForCLMutex *pT = new TestForCLMutex;
	pT->i = 0;
	
	pthread_t tid;
	
	pthread_create(&tid, 0, TestThreadForCLMutex, pT);

	for(int i = 0; i < 1000000; i++)
	{
		EXPECT_TRUE((pT->mutex.Lock()).IsSuccess());
		pT->i = pT->i + 1;
		EXPECT_TRUE((pT->mutex.Unlock()).IsSuccess());
	}

	pthread_join(tid, 0);

	EXPECT_EQ(pT->i, 2000000);

	delete pT;
}

static long g_bforCLmutex = 0;

void* TestThreadForCLMutex2(void *arg)
{
	CLMutex *pT = (CLMutex *)arg;

	for(int i = 0; i < 1000000; i++)
	{
		EXPECT_TRUE((pT->Lock()).IsSuccess());
		g_bforCLmutex++;
		EXPECT_TRUE((pT->Unlock()).IsSuccess());
	}

	return 0;
}

TEST(CLMutex, Normal2)
{
	pthread_mutex_t pmutex = PTHREAD_MUTEX_INITIALIZER;

	CLMutex mutex(&pmutex);

	pthread_t tid;

	pthread_create(&tid, 0, TestThreadForCLMutex2, &mutex);

	for(int i = 0; i < 1000000; i++)
	{
		EXPECT_TRUE((mutex.Lock()).IsSuccess());
		g_bforCLmutex++;
		EXPECT_TRUE((mutex.Unlock()).IsSuccess());
	}

	pthread_join(tid, 0);

	EXPECT_EQ(g_bforCLmutex, 2000000);

	g_bforCLmutex = 0;
}

void* TestThreadForCLMutex3(void *arg)
{
	pthread_mutex_t *pmutex =(pthread_mutex_t *)arg;

	CLMutex mutex(pmutex);

	for(int i = 0; i < 1000000; i++)
	{
		EXPECT_TRUE((mutex.Lock()).IsSuccess());
		g_bforCLmutex++;
		EXPECT_TRUE((mutex.Unlock()).IsSuccess());
	}

	return 0;
}

TEST(CLMutex, Normal3)
{
	pthread_mutex_t pmutex = PTHREAD_MUTEX_INITIALIZER;

	CLMutex mutex(&pmutex);

	pthread_t tid;

	pthread_create(&tid, 0, TestThreadForCLMutex3, &pmutex);

	for(int i = 0; i < 1000000; i++)
	{
		EXPECT_TRUE((mutex.Lock()).IsSuccess());
		g_bforCLmutex++;
		EXPECT_TRUE((mutex.Unlock()).IsSuccess());
	}

	pthread_join(tid, 0);

	EXPECT_EQ(g_bforCLmutex, 2000000);
}

TEST(CLMutex, RecordLock)
{
	int fd = open("test_for_record_lock", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	long i = 0;
	write(fd, &i, sizeof(long));

	pid_t pid = fork();
	if(pid == 0)
	{
		{
			CLMutex mutex("testforrecordlock", MUTEX_USE_RECORD_LOCK);

			for(int j = 0; j < 1000000; j++)
			{
				mutex.Lock();

				lseek(fd, SEEK_SET, 0);
				long k = 0;
				read(fd, &k, sizeof(long));

				lseek(fd, SEEK_SET, 0);
				k++;
				write(fd, &k, sizeof(long));

				mutex.Unlock();
			}

			close(fd);

			CLLibExecutiveInitializer::Destroy();
		}

		exit(0);
	}
	
	{
		CLMutex mutex("testforrecordlock", MUTEX_USE_RECORD_LOCK);

		for(int j = 0; j < 1000000; j++)
		{
			mutex.Lock();

			lseek(fd, SEEK_SET, 0);
			long k = 0;
			read(fd, &k, sizeof(long));

			lseek(fd, SEEK_SET, 0);
			k++;
			write(fd, &k, sizeof(long));

			mutex.Unlock();
		}
	}
	
	waitpid(pid, 0, 0);

	long k = 0;
	lseek(fd, SEEK_SET, 0);
	read(fd, &k, sizeof(long));

	close(fd);

	EXPECT_EQ(k, 2000000);
}

struct SLForRecordAndPthread
{
	int fd;
	CLMutex *pmutex;
};

void *thread_for_record_and_pthread(void *arg)
{
	SLForRecordAndPthread *p = (SLForRecordAndPthread *)arg;

	for(int j = 0; j < 1000000; j++)
	{
		p->pmutex->Lock();

		lseek(p->fd, SEEK_SET, 0);
		long k = 0;
		read(p->fd, &k, sizeof(long));

		lseek(p->fd, SEEK_SET, 0);
		k++;
		write(p->fd, &k, sizeof(long));

		p->pmutex->Unlock();
	}
	return 0;
}

TEST(CLMutex, RecordLockAndPthread)
{
	int fd = open("test_for_record_lock_and_pthread", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	long i = 0;
	write(fd, &i, sizeof(long));

	pid_t pid = fork();
	if(pid == 0)
	{
		{
			CLMutex mutex("mutex_forrecord_and_pthread", MUTEX_USE_RECORD_LOCK_AND_PTHREAD);
			SLForRecordAndPthread sl;
			sl.fd = fd;
			sl.pmutex = &mutex;

			pthread_t tid;
			pthread_create(&tid, 0, thread_for_record_and_pthread, &sl);

			for(int j = 0; j < 1000000; j++)
			{
				mutex.Lock();

				lseek(fd, SEEK_SET, 0);
				long k = 0;
				read(fd, &k, sizeof(long));

				lseek(fd, SEEK_SET, 0);
				k++;
				write(fd, &k, sizeof(long));

				mutex.Unlock();
			}

			pthread_join(tid, 0);

			close(fd);

			CLLibExecutiveInitializer::Destroy();
		}

		exit(0);
	}

	{
		CLMutex mutex("mutex_forrecord_and_pthread", MUTEX_USE_RECORD_LOCK_AND_PTHREAD);
		SLForRecordAndPthread sl;
		sl.fd = fd;
		sl.pmutex = &mutex;

		pthread_t tid;
		pthread_create(&tid, 0, thread_for_record_and_pthread, &sl);

		for(int j = 0; j < 1000000; j++)
		{
			mutex.Lock();

			lseek(fd, SEEK_SET, 0);
			long k = 0;
			read(fd, &k, sizeof(long));

			lseek(fd, SEEK_SET, 0);
			k++;
			write(fd, &k, sizeof(long));

			mutex.Unlock();
		}

		pthread_join(tid, 0);
	}

	waitpid(pid, 0, 0);

	long k = 0;
	lseek(fd, SEEK_SET, 0);
	read(fd, &k, sizeof(long));

	close(fd);

	EXPECT_EQ(k, 4000000);
}

struct SLForRecordAndPthread2
{
	int fd;
	pthread_mutex_t *pmutex;
};

void *thread_for_record_and_pthread2(void *arg)
{
	SLForRecordAndPthread2 *p = (SLForRecordAndPthread2 *)arg;

	for(int j = 0; j < 1000000; j++)
	{
		CLMutex mutex("mutex_forrecord_and_pthread2", p->pmutex);
		CLCriticalSection cs(&mutex);

		lseek(p->fd, SEEK_SET, 0);
		long k = 0;
		read(p->fd, &k, sizeof(long));

		lseek(p->fd, SEEK_SET, 0);
		k++;
		write(p->fd, &k, sizeof(long));
	}
	return 0;
}

TEST(CLMutex, RecordLockAndPthread2)
{
	int fd = open("test_for_record_lock_and_pthread2", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	long i = 0;
	write(fd, &i, sizeof(long));

	pthread_mutex_t pmutex = PTHREAD_MUTEX_INITIALIZER;

	pid_t pid = fork();
	if(pid == 0)
	{
		SLForRecordAndPthread2 s;
		s.fd = fd;
		s.pmutex = &pmutex;

		pthread_t tid;
		pthread_create(&tid, 0, thread_for_record_and_pthread2, &s);

		for(int j = 0; j < 1000000; j++)
		{
			CLMutex mutex("mutex_forrecord_and_pthread2", &pmutex);
			CLCriticalSection cs(&mutex);

			lseek(fd, SEEK_SET, 0);
			long k = 0;
			read(fd, &k, sizeof(long));

			lseek(fd, SEEK_SET, 0);
			k++;
			write(fd, &k, sizeof(long));
		}

		pthread_join(tid, 0);

		close(fd);

		CLLibExecutiveInitializer::Destroy();

		exit(0);
	}

	{
		SLForRecordAndPthread2 s;
		s.fd = fd;
		s.pmutex = &pmutex;

		pthread_t tid;
		pthread_create(&tid, 0, thread_for_record_and_pthread2, &s);

		for(int j = 0; j < 1000000; j++)
		{
			CLMutex mutex("mutex_forrecord_and_pthread2", &pmutex);
			CLCriticalSection cs(&mutex);

			lseek(fd, SEEK_SET, 0);
			long k = 0;
			read(fd, &k, sizeof(long));

			lseek(fd, SEEK_SET, 0);
			k++;
			write(fd, &k, sizeof(long));
		}

		pthread_join(tid, 0);
	}

	waitpid(pid, 0, 0);

	long k = 0;
	lseek(fd, SEEK_SET, 0);
	read(fd, &k, sizeof(long));

	close(fd);

	EXPECT_EQ(k, 4000000);
}

void *thread_for_shared_mutex_bypthread(void *arg)
{
	long *pg = (long *)arg;

	for(int i = 0; i < 1000000; i++)
	{
		CLMutex mutex("mutex_for_CLMutex_SharedMutexByPthread", MUTEX_USE_SHARED_PTHREAD);
		CLCriticalSection cs(&mutex);

		(*pg) = (*pg) + 1;
	}

	return 0;
}

TEST(CLMutex, SharedMutexByPthread)
{
	CLSharedMemory sm("test_for_CLMutex_SharedMutexByPthread", 8);
	long *pg = (long *)sm.GetAddress();
	*pg = 0;

	pid_t pid = fork();
	if(pid == 0)
	{
		pthread_t tid;
		pthread_create(&tid, 0, thread_for_shared_mutex_bypthread, pg);

		for(int i = 0; i < 1000000; i++)
		{
			CLMutex mutex("mutex_for_CLMutex_SharedMutexByPthread", MUTEX_USE_SHARED_PTHREAD);
			CLCriticalSection cs(&mutex);

			(*pg) = (*pg) + 1;
		}

		pthread_join(tid, 0);

		_exit(0);
	}

	pthread_t tid;
	pthread_create(&tid, 0, thread_for_shared_mutex_bypthread, pg);

	for(int i = 0; i < 1000000; i++)
	{
		CLMutex mutex("mutex_for_CLMutex_SharedMutexByPthread", MUTEX_USE_SHARED_PTHREAD);
		CLCriticalSection cs(&mutex);

		(*pg) = (*pg) + 1;
	}

	pthread_join(tid, 0);

	waitpid(pid, 0, 0);

	EXPECT_EQ(*pg, 4000000);
}