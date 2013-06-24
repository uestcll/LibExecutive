#include <gtest/gtest.h>
#include "CLLibExecutiveInitializer.h"
#include "CLLogger.h"

#define NUM_LIB 300

void* TestThreadForCLLogDestroy(void *arg)
{	
	long i = (long)arg;
	if(i % 10 == 8)
	{
		CLLibExecutiveInitializer::Destroy();
		EXPECT_TRUE(CLLogger::GetInstance() == 0);
		return 0;
	}

	CLLogger::WriteLogMsg("abcdefghijklmnopqrst   ", i);
	return 0;
}

TEST(CLLibExecutiveInitializer1, Normal)
{
	pthread_t tid[NUM_LIB];	
	for(long i = 0; i < NUM_LIB; i++)	
	{		
		pthread_create(&(tid[i]), 0, TestThreadForCLLogDestroy, (void *)i);
	}	

	for(long i = 0; i < NUM_LIB; i++)
	{		
		pthread_join(tid[i], 0);	
	}
}

TEST(CLLibExecutiveInitializer1, DestroyAgain)
{
	EXPECT_FALSE(CLLibExecutiveInitializer::Destroy().IsSuccess());
}

TEST(CLLibExecutiveInitializer1, InitialAgain)
{
	EXPECT_FALSE(CLLibExecutiveInitializer::Initialize().IsSuccess());
}