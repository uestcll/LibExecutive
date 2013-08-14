#include <gtest/gtest.h>
#include <pthread.h>
#include <unistd.h>
#include "LibExecutive.h"

struct TestForCLCriticalSection
{
	CLMutex mutex;
	int i;
};

static const int count = 1000000;

static void* TestThreadForCLCriticalSection(void *arg)
{
	TestForCLCriticalSection *pT = (TestForCLCriticalSection *)arg;
	
	for(int i = 0; i < count; i++)
	{
		{
			CLCriticalSection cs(&(pT->mutex));
			
			pT->i = pT->i + 1;
		}
	}
	
	return 0;
}

TEST(CLCriticalSection, Normal)
{
	TestForCLCriticalSection *pT = new TestForCLCriticalSection;
	pT->i = 0;
	
	pthread_t tid;	
	pthread_create(&tid, 0, TestThreadForCLCriticalSection, pT);

	TestThreadForCLCriticalSection(pT);

	pthread_join(tid, 0);

	EXPECT_EQ(pT->i, 2*count);

	delete pT;
}