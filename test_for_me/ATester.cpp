#include <gtest/gtest.h>
#include "LibExecutive.h"

#define NUM 30

void* TestThreadForCLLibExecutiveInitializerDestroy(void *arg)
{	
	CLLibExecutiveInitializer::Destroy();

	return 0;
}

TEST(CLLibExecutiveInitializerDestroy, Normal)
{
	pthread_t tid[NUM];	
	for(long i = 0; i < NUM; i++)	
	{		
		pthread_create(&(tid[i]), 0, TestThreadForCLLibExecutiveInitializerDestroy, 0);
	}	

	for(long i = 0; i < NUM; i++)
	{		
		pthread_join(tid[i], 0);
	}

	EXPECT_EQ(CLLogger::GetInstance(), (void *)0);
	EXPECT_EQ(CLExecutiveNameServer::GetInstance(), (void *)0);
}

TEST(CLLibExecutiveInitializerDestroy, DestroyAgain)
{
	EXPECT_FALSE(CLLibExecutiveInitializer::Destroy().IsSuccess());
}

TEST(CLLibExecutiveInitializerDestroy, InitialAgain)
{
	EXPECT_FALSE(CLLibExecutiveInitializer::Initialize().IsSuccess());
}