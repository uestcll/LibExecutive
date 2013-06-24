#include <gtest/gtest.h>
#include <pthread.h>
#include <unistd.h>
#include "CLCriticalSection.h"
#include "CLMutex.h"

struct TestForCLCriticalSection
{
	CLMutex mutex;
	int i;
};

void* TestThreadForCLCriticalSection(void *arg)
{
	TestForCLCriticalSection *pT = (TestForCLCriticalSection *)arg;
	
	for(int i = 0; i < 1000000; i++)
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

	for(int i = 0; i < 1000000; i++)
	{
		{
			CLCriticalSection cs(&(pT->mutex));
			
			pT->i = pT->i + 1;
		}
	}

	pthread_join(tid, 0);

	EXPECT_EQ(pT->i, 2000000);

	delete pT;
}
